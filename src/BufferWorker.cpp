#include <QFile>
#include <QMutexLocker>
#include <BufferWorker.h>
#include <srchilite/instances.h>
#include <srchilite/langmap.h>

// debug functions
QDebug operator<<(QDebug dbg, const BufferLineState *lineState)
{
    if (!lineState) {
        dbg << "<nil>";
    } else {
        dbg << *lineState;
    }
    return dbg;
}

BufferWorker::BufferWorker():
    _langMap(srchilite::Instances::getLangMap()),
    _sourceHighlight("default.style", "xhtml.outlang")
{
}

void BufferWorker::initialize()
{
    QMutexLocker locker(&_langMapMut);
    _langMap->open();
}

// TODO: finer locking here? we only need to exclude it from initialize's
// open call
QString BufferWorker::filetypeForName(const QString &name)
{
    QMutexLocker locker(&_langMapMut);
    return QString::fromUtf8(_langMap->getMappedFileNameFromFileName(name.toUtf8().constData()).c_str());
}

void BufferWorker::setFiletype(StateChangeContext &ctx, BufferState &state, const QString &filetype)
{
    QMutexLocker locker(&_mut);
    _filetype = filetype;
    qDebug() << "Setting filetype to:" << _filetype;
    if (_filetype.isEmpty()) {
        _mainStateData.reset();
    } else {
        _sourceHighlight.setInputLang(std::string(_filetype.toUtf8().constData()) + ".lang");
        // initialize the main state data
        _mainStateData = HighlightStateData::ptr(new HighlightStateData(
                _sourceHighlight.getHighlighter()->getMainState(),
                srchilite::HighlightStateStackPtr(new srchilite::HighlightStateStack())
        ));
    }
    _highlight(state, 0, _mainStateData);
    qDebug() << "sending new state update";
    emit filetypeChanged(ctx, state);
}

// the orthogonal highlighting procedure: compare the start state and if different, keep on highlighting
void BufferWorker::_highlight(BufferState &state, int index, HighlightStateData::ptr highlightState, HighlightStateData::ptr oldHighlightState, float startProgress, float endProgress)
{
    float progressInc = (endProgress-startProgress) / qMax(state.size()-index, 1);
    if (state.filetype() != _filetype) {
        state.setFiletype(_filetype);
        // highlight without checking
        for (; index < state.size(); index++) {
            qDebug() << "highlighting line" << index;
            highlightState = highlightLine(state[index], highlightState);
            emit progressChanged(startProgress+=progressInc);
        }
    } else {
        for (; index < state.size() && HighlightStateData::unequal(highlightState, oldHighlightState); index++) {
            oldHighlightState = state[index].endHighlightState;
            qDebug() << "highlighting line" << index;
            highlightState = highlightLine(state[index], highlightState);
            emit progressChanged(startProgress+=progressInc);
        }
    }
    emit progressChanged(endProgress);
}

HighlightStateData::ptr BufferWorker::highlightLine(BufferLineState &lineState, HighlightStateData::ptr highlightState)
{
    if (_filetype.isEmpty() || lineState.line.isEmpty() || !highlightState) {
        lineState.highlightText.clear();
        lineState.endHighlightState = highlightState;
    } else {
        HighlightStateData::ptr state = HighlightStateData::ptr(new HighlightStateData(*highlightState));
        _sourceHighlight.getHighlighter()->setCurrentState(state->currentState);
        _sourceHighlight.getHighlighter()->setStateStack(state->stateStack);
        _sourceHighlight.clearBuffer();
        QByteArray plainText;
        QTextStream stream(&plainText);
        lineState.line.writePlainText(stream);
        stream << flush;
        _sourceHighlight.getHighlighter()->highlightParagraph(std::string(plainText.constData()));
        lineState.highlightText = QString::fromUtf8(_sourceHighlight.getBuffer().str().c_str());
        state->currentState = _sourceHighlight.getHighlighter()->getCurrentState();
        lineState.endHighlightState = state;
    }
    return lineState.endHighlightState;
}

BufferStateChange BufferWorker::parseBufferChange(BufferState &state, const QString &content, ParserPosition start, int cursorPosition)
{
    QMutexLocker locker(&_mut);
    return _parseBufferChange(state, content, start, cursorPosition);
}


BufferStateChange BufferWorker::_parseBufferChange(BufferState &state, const QString &content, ParserPosition start, int cursorPosition)
{
    state.setCursorPosition(cursorPosition);
    return _bufferChangeParser.parseBufferChange(content, start, cursorPosition);
}

void BufferWorker::parseAndMergeChange(StateChangeContext &ctx, BufferState &state, const QString &content, ParserPosition start, int cursorPosition, bool trackProgress)
{
    QMutexLocker locker(&_mut);
    _mergeChange(ctx, state, _parseBufferChange(state, content, start, cursorPosition), trackProgress);
}

void BufferWorker::mergeChange(StateChangeContext &ctx, BufferState &state, const BufferStateChange &change, bool trackProgress)
{
    QMutexLocker locker(&_mut);
    _mergeChange(ctx, state, change, trackProgress);
}

void BufferWorker::_mergeChange(StateChangeContext &ctx, BufferState &state, const BufferStateChange &change, bool trackProgress)
{
    // if the filetype is empty then we don't need to track progress
    float currentProgress = 0, progressInc = trackProgress ? 1.0 / (change.size()+1) : 0;
    if (state.filetype() != _filetype) {
        Q_ASSERT(state.isEmpty());
        state.setFiletype(_filetype);
    }
    // pull in the changes
    int bufferIndex = qMax(change.startIndex(), 0);
    int changeIndex = -1;
    int offset = 0;
    HighlightStateData::ptr currentHighlightData;
    int beforeStartIndex = bufferIndex - 1;
    if (beforeStartIndex >= 0 && beforeStartIndex < state.size()) {
        currentHighlightData = state[beforeStartIndex].endHighlightState;
    } else {
        currentHighlightData = _mainStateData;
    }
    HighlightStateData::ptr lastEndHighlightData = currentHighlightData;
    for (int i = 0; i < change.size(); i++) {
        BufferLineState lineState(change[i].line);
        currentHighlightData = highlightLine(lineState, currentHighlightData);
        changeIndex = change[i].index;
        if (changeIndex < 0) {
            // we need to insert this new line
            state.insert(bufferIndex, lineState);
            offset++;
        } else {
            // we need to delete the lines until the index match up
            while (bufferIndex != changeIndex+offset) {
                state.removeAt(bufferIndex);
                offset--;
            }
            lastEndHighlightData = state[bufferIndex].endHighlightState;
            state[bufferIndex] = lineState;
        }
        bufferIndex++;
        emit progressChanged(currentProgress+=progressInc);
    }
    // if there is no merge edge then we can remove all the rest
    if (changeIndex < 0) {
        while (bufferIndex < state.size()) {
            state.removeLast();
        }
        emit progressChanged(1);
    } else {
        qDebug() << "continuing to highlight additional lines";
        _highlight(state, bufferIndex, currentHighlightData, lastEndHighlightData, currentProgress);
    }
    // if there is nothing left.
    // there can be no highlight change when the buffer is empty
    if (state.size() == 1 && state[0].line.isEmpty()) {
        state.removeAt(0);
        ctx.sourceViewShouldUpdate = false;
    } else {
        ctx.sourceViewShouldUpdate = !_filetype.isEmpty() && !change.delayable();
    }
    emit changeMerged(ctx, state);
}

// TODO: set cursorPosition of the state to the end of last Replacement
// potentially needing to record a second counter
void BufferWorker::replace(StateChangeContext &ctx, BufferState &state, const QList<Replacement> &replaces)
{
    QMutexLocker locker(&_mut);
    float currentProgress = 0, progressInc = 1.0 / (replaces.size()+1);
    if (state.filetype() != _filetype) {
        Q_ASSERT(state.isEmpty());
        state.setFiletype(_filetype);
    }
    int charCount = 0;
    int stateIndex = 0;
    BufferLineState temp; // used as a temp storage for pointers
    BufferLineState *current = &state[stateIndex], *before = NULL, *after = NULL;
    HighlightStateData::ptr highlightState = _mainStateData, oldHighlightState = _mainStateData;
    for (int i = 0; i < replaces.size(); i++) {
        const Replacement &rep = replaces[i];
        qDebug() << "replacing" << rep;
        // fast forward until reaching the replace start
        while (true) {
            qDebug() << "charCount:" << charCount << "stateIndex:" << stateIndex;
            qDebug() << "current:" << current;
            qDebug() << "before:" << before;
            qDebug() << "after:" << after;
            if (charCount + current->line.size() < rep.selection.start) {
                qDebug() << "skippinng the current line";
                charCount += current->line.size() + 1;
            } else {
                qDebug() << "splitting the current line";
                temp = BufferLineState(current->line.split(rep.selection.start - charCount), current->endHighlightState);
                qDebug() << "updating the charCount to match the start of selection";
                charCount = rep.selection.start;
                after = &temp;
                qDebug() << "current:" << current;
                qDebug() << "after:" << after;
            }
            if (before) {
                qDebug() << "appending current to before";
                before->line.append(current->line);
                before->endHighlightState = current->endHighlightState;
                current = before;
                before = NULL;
                qDebug() << "current:" << current;
                qDebug() << "before:" << before;
            }
            if (after) {
                qDebug() << "resetting oldHighlightState";
                // force highlight
                oldHighlightState.reset();
                break;
            }
            if (HighlightStateData::unequal(oldHighlightState, highlightState)) {
                oldHighlightState = current->endHighlightState;
                qDebug() << "highlighting current";
                highlightState = highlightLine(*current, highlightState);
            } else {
                qDebug() << "setting highlight state to the end state of current";
                highlightState = oldHighlightState = current->endHighlightState;
            }
            qDebug() << "advancing the stateIndex and pointing current to the next line in state";
            current = &state[++stateIndex];
        }
        // start inserting the replaced content
        qDebug() << "inserting replacement into state...";
        for (int j = 0; j < rep.replacement.size(); j++) {
            QChar ch = rep.replacement[j];
            if (ch == '\n' || ch == '\r') {
                qDebug() << "highlighting current";
                highlightState = highlightLine(*current, highlightState);
                state.insert(++stateIndex, BufferLineState());
                qDebug() << "newline met: advancing stateIndex to" << stateIndex;
                qDebug() << "inserting a new BufferLineState at the new stateIndex and pointing current to it";
                current = &state[stateIndex];
            } else {
                current->line.append(ch);
            }
        }
        qDebug() << "finished insertion";
        qDebug() << "current:" << current;
        qDebug() << "before:" << before;
        qDebug() << "after:" << after;
        qDebug() << "shifting forward, pointing before to current, current to after";
        before = current;
        current = after;
        after = NULL;
        qDebug() << "current:" << current;
        qDebug() << "before:" << before;
        qDebug() << "eating away the length of the original selection...";
        qDebug() << "stateIndex:" << stateIndex;
        // eat away the length of the replacement
        while (charCount + current->line.size() < rep.selection.end) {
            qDebug() << "charCount:" << charCount;
            qDebug() << "skippinng the current line";
            charCount += current->line.size() + 1;
            temp = state[stateIndex+1];
            qDebug() << "detaching from index:" << stateIndex+1 << "line:" << temp;
            qDebug() << "removing line at index:" << stateIndex+1;
            state.removeAt(stateIndex+1);
            qDebug() << "pointing current to the detached line";
            current = &temp;
        }
        qDebug() << "charCount:" << charCount;
        // split again to remove the replaced part
        temp = BufferLineState(current->line.split(rep.selection.end - charCount), current->endHighlightState);
        qDebug() << "splitting again to remove the final part of the selection, split:" << temp;
        qDebug() << "pointing current to split";
        current = &temp;
        emit progressChanged(currentProgress+=progressInc);
    }
    qDebug() << "finished all the replacements";
    qDebug() << "current:" << current;
    qDebug() << "before:" << before;
    qDebug() << "appending current to the end of before";
    before->line.append(current->line);
    qDebug() << "appended current to before";
    qDebug() << "before:" << before;
    before->endHighlightState = current->endHighlightState;
    qDebug() << "final state:" << state;
    before->endHighlightState = current->endHighlightState;
    qDebug() << "continuing to highlight";
    qDebug() << "pointing current to before";
    _highlight(state, stateIndex, highlightState, oldHighlightState, currentProgress);
    emit occurrenceReplaced(ctx, state);
}

void BufferWorker::rehighlight(StateChangeContext &ctx, BufferState &state, int index)
{
    QMutexLocker locker(&_mut);
    _highlight(state, index, index == 0 ? _mainStateData : state[index-1].endHighlightState);
    emit stateRehighlighted(ctx, state);
}

void BufferWorker::writePlainText(const BufferState &state, QTextStream &output, float startProgress, float endProgress)
{
    float progressInc = (endProgress-startProgress) / qMax(state.size(), 1);
    if (state.empty())
       return;
    state[0].line.writePlainText(output);
    emit progressChanged(startProgress+=progressInc);
    for (int i = 1; i < state.size(); i++) {
        output << '\n';
        state[i].line.writePlainText(output);
        emit progressChanged(startProgress+=progressInc);
    }
    emit progressChanged(endProgress);
}

void BufferWorker::saveStateToFile(const BufferState &state, const QString &filename)
{
    float currentProgress = 0;
    QFile file(filename);
    qDebug() << "preparing to write to " << filename;
    bool success = file.open(QIODevice::WriteOnly | QIODevice::Text);
    emit progressChanged(currentProgress += 0.25);
    if (!success) {
        emit progressChanged(currentProgress,
                bb::cascades::ProgressIndicatorState::Error, tr("Error opening file"));
        return;
    }
    QTextStream out(&file);
    writePlainText(state, out, currentProgress);
    file.close();
    emit progressChanged(1);
}

void BufferWorker::loadStateFromFile(StateChangeContext &ctx, const QString &filename)
{
//    return BufferState();
}
