#include <QFile>
#include <QMutexLocker>
#include <srchilite/instances.h>
#include <srchilite/langmap.h>
#include <BufferWorker.h>
#include <FiletypeMap.h>
#include <Filetype.h>
#include <Utility.h>
#include <Helium.h>
#include <AppearanceSettings.h>

BufferWorker::BufferWorker():
    _sourceHighlight("xhtml.outlang")
{
}

void BufferWorker::setHighlightType(StateChangeContext &ctx, BufferState &state, const HighlightType &highlightType, Progress progress)
{
    QMutexLocker locker(&_mut);
    _setHighlightType(highlightType);
    _highlight(state, 0, _mainStateData, HighlightStateData::ptr(), progress);
    qDebug() << "sending new state update";
    emit highlightTypeChanged(ctx, state);
}

void BufferWorker::_setHighlightType(const HighlightType &type)
{
    if (type != _highlightType) {
        _highlightType = type;
        qDebug() << "Setting filetype to:" << _highlightType.filetype;
        if (_highlightType.shouldHighlight()) {
            _sourceHighlight.setStyleFile(_highlightType.styleFile.toStdString());
            _sourceHighlight.setInputLang(_highlightType.filetype->name().toStdString() + ".lang");
            _mainStateData = HighlightStateData::ptr(new HighlightStateData(
                _sourceHighlight.getHighlighter()->getMainState(),
                srchilite::HighlightStateStackPtr(new srchilite::HighlightStateStack())
            ));
        } else {
            _mainStateData.reset();
        }
    }
}

// the orthogonal highlighting procedure: compare the start state and if different, keep on highlighting
void BufferWorker::_highlight(BufferState &state, int index, HighlightStateData::ptr highlightState, HighlightStateData::ptr oldHighlightState, Progress progress)
{
    float progressInc = (progress.cap-progress.current) / qMax(state.size()-index, 1);
    if (state.highlightType() != _highlightType) {
        state.setHighlightType(_highlightType);
        // highlight without checking
        for (; index < state.size(); index++) {
            qDebug() << "highlighting line" << index;
            highlightState = highlightLine(state[index], highlightState);
            emit progressChanged(progress.current+=progressInc);
        }
    } else {
        for (; index < state.size() && HighlightStateData::unequal(highlightState, oldHighlightState); index++) {
            oldHighlightState = state[index].endHighlightState;
            qDebug() << "highlighting line" << index;
            highlightState = highlightLine(state[index], highlightState);
            emit progressChanged(progress.current+=progressInc);
        }
    }
    emit progressChanged(progress.cap);
}

HighlightStateData::ptr BufferWorker::highlightLine(BufferLineState &lineState, HighlightStateData::ptr highlightState)
{
    if (!_highlightType.shouldHighlight() || lineState.line.isEmpty() || !highlightState) {
        lineState.highlightText.clear();
        lineState.endHighlightState = highlightState;
    } else {
        HighlightStateData::ptr state = HighlightStateData::ptr(new HighlightStateData(*highlightState));
        _sourceHighlight.getHighlighter()->setCurrentState(state->currentState);
        _sourceHighlight.getHighlighter()->setStateStack(state->stateStack);
        QByteArray plainText;
        QTextStream stream(&plainText);
        lineState.line.writePlainText(stream);
        stream << flush;
        _sourceHighlight.getHighlighter()->highlightParagraph(std::string(plainText.constData()));
        lineState.highlightText = QString::fromStdString(_sourceHighlight.getBuffer().str());
        _sourceHighlight.clearBuffer();
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

void BufferWorker::parseAndMergeChange(StateChangeContext &ctx, BufferState &state, const QString &content, ParserPosition start, int cursorPosition, Progress progress)
{
    QMutexLocker locker(&_mut);
    _mergeChange(ctx, state, _parseBufferChange(state, content, start, cursorPosition), progress);
}

void BufferWorker::mergeChange(StateChangeContext &ctx, BufferState &state, const BufferStateChange &change, Progress progress)
{
    QMutexLocker locker(&_mut);
    _mergeChange(ctx, state, change, progress);
}

void BufferWorker::_mergeChange(StateChangeContext &ctx, BufferState &state, const BufferStateChange &change, Progress progress)
{
    // if the filetype is empty then we don't need to track progress
    float progressInc = (progress.cap-progress.current) / (change.size()+1);
    if (state.highlightType() != _highlightType) {
        Q_ASSERT(state.isEmpty());
        state.setHighlightType(_highlightType);
    }
    // pull in the changes
    int bufferIndex = qMax(change.startIndex(), 0);
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
        int changeIndex = change[i].index;
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
        emit progressChanged(progress.current+=progressInc);
    }
    // if this change concludes the entire buffer then we can remove all the rest
    if (change.atEnd()) {
        while (bufferIndex < state.size()) {
            state.removeLast();
        }
        emit progressChanged(progress.cap);
    } else {
        qDebug() << "continuing to highlight additional lines";
        _highlight(state, bufferIndex, currentHighlightData, lastEndHighlightData, progress);
    }
    // if there is nothing left.
    // there can be no highlight change when the buffer is empty
    if (state.size() == 1 && state[0].line.isEmpty()) {
        state.removeAt(0);
        ctx.sourceViewShouldUpdate = false;
    } else {
        ctx.sourceViewShouldUpdate = _highlightType.shouldHighlight() && !change.delayable();
    }
    emit changeMerged(ctx, state);
}

// TODO: set cursorPosition of the state to the end of last Replacement
// potentially needing to record a second counter
void BufferWorker::replace(StateChangeContext &ctx, BufferState &state, const QList<Replacement> &replaces, Progress progress)
{
    QMutexLocker locker(&_mut);
    float progressInc = (progress.cap-progress.current) / (replaces.size()+1);
    if (state.highlightType() != _highlightType) {
        Q_ASSERT(state.isEmpty());
        state.setHighlightType(_highlightType);
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
            qDebug() << "before:" << before;
            qDebug() << "current:" << current;
            qDebug() << "after:" << after;
            if (charCount + current->line.size() < rep.selection.start) {
                qDebug() << "skipping the current line";
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
                qDebug() << "before:" << before;
                qDebug() << "current:" << current;
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
        qDebug() << "before:" << before;
        qDebug() << "current:" << current;
        qDebug() << "after:" << after;
        qDebug() << "shifting forward, pointing before to current, current to after";
        before = current;
        current = after;
        after = NULL;
        qDebug() << "before:" << before;
        qDebug() << "current:" << current;
        qDebug() << "eating away the length of the original selection...";
        qDebug() << "stateIndex:" << stateIndex;
        // eat away the length of the replacement
        while (charCount + current->line.size() < rep.selection.end) {
            qDebug() << "charCount:" << charCount;
            qDebug() << "skipping the current line";
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
        qDebug() << "updating the charCount to match the end of selection";
        charCount = rep.selection.end;
        qDebug() << "pointing current to split";
        current = &temp;
        emit progressChanged(progress.current+=progressInc);
    }
    qDebug() << "finished all the replacements";
    qDebug() << "before:" << before;
    qDebug() << "current:" << current;
    qDebug() << "appending current to the end of before";
    before->line.append(current->line);
    qDebug() << "appended current to before";
    qDebug() << "before:" << before;
    before->endHighlightState = current->endHighlightState;
    qDebug() << "final state:" << state;
    qDebug() << "continuing to highlight";
    qDebug() << "pointing current to before";
    _highlight(state, stateIndex, highlightState, oldHighlightState, progress);
    emit occurrenceReplaced(ctx, state);
}

void BufferWorker::rehighlight(StateChangeContext &ctx, BufferState &state, int index, Progress progress)
{
    QMutexLocker locker(&_mut);
    _highlight(state, index, index == 0 ? _mainStateData : state[index-1].endHighlightState, HighlightStateData::ptr(), progress);
    emit stateRehighlighted(ctx, state);
}

void writeLineWithTabToSpaceConversion(const BufferLine &line, QTextStream &output, int numberOfSpacesForTab)
{
    QString raw = line.plainText();
    int i = 0;
    for (; i < raw.size(); i++) {
        if (raw[i] != '\t')
            break;
        for (int j = 0; j < numberOfSpacesForTab; j++) {
            output << ' ';
        }
    }
    for (; i < raw.size(); i++) {
        output << raw[i];
    }
}

void BufferWorker::saveStateToFile(const BufferState &state, const QString &filename, Progress progress)
{
    QFile file(filename);
    qDebug() << "preparing to write to" << filename;
    emit progressChanged(progress.current+=(progress.cap-progress.current)*0.10);
    bool success = file.open(QIODevice::WriteOnly | QIODevice::Text);
    progress.current += (progress.cap-progress.current)*0.25;
    if (success) {
        emit progressChanged(progress.current);
    } else {
        emit progressChanged(progress.current, bb::cascades::ProgressIndicatorState::Error);
        Utility::toast(tr("Error opening file"), tr("OK"), this, SIGNAL(progressDismissed()));
        return;
    }
    if (!state.empty()) {
        QTextStream out(&file);
        float progressInc = (progress.cap-progress.current) / qMax(state.size(), 1);
        if (_highlightType.filetype &&
                _highlightType.filetype->tabSpaceConversionEnabled()) {
            int n = _highlightType.filetype->numberOfSpacesForTab();
            qDebug() << "converting each tab to" << n << "spaces";
            writeLineWithTabToSpaceConversion(state[0].line, out, n);
            emit progressChanged(progress.current+=progressInc);
            for (int i = 1; i < state.size(); i++) {
                out << '\n';
                writeLineWithTabToSpaceConversion(state[i].line, out, n);
                emit progressChanged(progress.current+=progressInc);
            }
        } else {
            state[0].line.writePlainText(out);
            emit progressChanged(progress.current+=progressInc);
            for (int i = 1; i < state.size(); i++) {
                out << '\n';
                state[i].line.writePlainText(out);
                emit progressChanged(progress.current+=progressInc);
            }
        }
        // NOTE: write newline at the end of file
        // http://stackoverflow.com/questions/729692/why-should-files-end-with-a-newline
        out << '\n';
    }
    emit progressChanged(progress.cap);
    emit stateSavedToFile(filename);
}

void BufferWorker::loadStateFromFile(StateChangeContext &ctx, const QString &filename, bool autodetectFiletype, Progress progress)
{
    QFile file(filename);
    qDebug() << "preparing to open" << filename;
    bool success = file.open(QIODevice::ReadOnly | QIODevice::Text);
    progress.current += (progress.cap-progress.current)*0.25;
    if (success) {
        emit progressChanged(progress.current);
    } else {
        emit progressChanged(progress.current, bb::cascades::ProgressIndicatorState::Error);
        Utility::toast(tr("Error opening file"), tr("OK"), this, SIGNAL(progressDismissed()));
        return;
    }
    if (autodetectFiletype) {
        _setHighlightType(HighlightType(Helium::instance()->appearance()->highlightStyleFile(),
                Helium::instance()->filetypeMap()->filetypeForName(filename)));
    }
    QTextStream input(&file);
    // build the initial state
    BufferState state;
    if (_highlightType.filetype &&
            _highlightType.filetype->tabSpaceConversionEnabled()) {
        Q_ASSERT(_highlightType.filetype->numberOfSpacesForTab() > 0);
        int limit = _highlightType.filetype->numberOfSpacesForTab() - 1;
        qDebug() << "converting" << limit+1 << "spaces to tabs...";
        while (!input.atEnd()) {
            QString raw = input.readLine();
            BufferLine line;
            int i = 0, nSpaces = 0;
            for (; i < raw.size(); i++) {
                QChar ch = raw[i];
                if (ch == ' ') {
                    if (nSpaces == limit) {
                        line << '\t';
                        nSpaces = 0;
                    } else {
                        nSpaces++;
                    }
                    continue;
                }
                if (nSpaces != 0) {
                    break;
                }
                if (ch == '\t') {
                    line << ch;
                    continue;
                }
                break;
            }
            for (int j = 0; j < nSpaces; j++) {
                line << ' ';
            }
            for (; i < raw.size(); i++) {
                line << raw[i];
            }
            state.append(BufferLineState(line));
        }
    } else {
        while (!input.atEnd())
            state.append(BufferLineState(BufferLine() << input.readLine()));
    }
    _highlight(state, 0, _mainStateData, HighlightStateData::ptr(), progress);
    qDebug() << "state filetype after highlight" << state.filetype();
    emit stateLoadedFromFile(ctx, state, filename);
}

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

QDebug operator<<(QDebug dbg, const Progress &progress)
{
    dbg.nospace() << "Progress(" << progress.current << "," << progress.cap << ")";
    return dbg.space();
}
