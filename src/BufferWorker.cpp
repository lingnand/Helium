#include <src/BufferWorker.h>

BufferWorker::BufferWorker():
    _sourceHighlight("default.style", "xhtml.outlang")
{
}

void BufferWorker::saveStateToFile(const BufferState &state, const QString &filename)
{

}

BufferState BufferWorker::loadStateFromFile(const QString &filename)
{
    return BufferState();
}

const QString &BufferWorker::filetype() const { return _filetype; }

bool BufferWorker::setFiletype(const QString &filetype)
{
    if (filetype != _filetype) {
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
        return true;
    }
    return false;
}

// when we don't have initial state, we deduce one
void BufferWorker::highlight(BufferState &state, int index)
{
    highlight(state, index, index == 0 ? _mainStateData : state[index-1].endHighlightState);
}

// the orthogonal highlighting procedure: compare the start state and if different, keep on highlighting
void BufferWorker::highlight(BufferState &state, int index, HighlightStateData::ptr highlightState, HighlightStateData::ptr oldHighlightState)
{
    if (state.filetype() != filetype()) {
        state.setFiletype(filetype());
        // highlight without checking
        for (; index < state.size(); index++) {
            qDebug() << "highlighting line" << index;
            highlightState = highlightLine(state[index], highlightState);
        }
    } else {
        for (; index < state.size() && HighlightStateData::unequal(highlightState, oldHighlightState); index++) {
            oldHighlightState = state[index].endHighlightState;
            qDebug() << "highlighting line" << index;
            highlightState = highlightLine(state[index], highlightState);
        }
    }
}

HighlightStateData::ptr BufferWorker::highlightLine(BufferLineState &lineState, HighlightStateData::ptr highlightState)
{
    if (filetype().isEmpty() || lineState.line.isEmpty() || !highlightState) {
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

void BufferWorker::mergeChange(BufferState &state, View *source, const BufferStateChange &change)
{
    float currentProgress = 0, progressInc = 1.0 / (change.size()+1);
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
        emit inProgressChanged(currentProgress+=progressInc);
    }
    // if there is no merge edge then we can remove all the rest
    if (changeIndex < 0) {
        while (bufferIndex < state.size()) {
            state.removeLast();
        }
    } else {
        qDebug() << "continuing to highlight additional lines";
        highlight(state, bufferIndex, currentHighlightData, lastEndHighlightData);
    }
    bool sourceChanged = false;
    // if there is nothing left.
    // there can be no highlight change when the buffer is empty
    if (state.size() == 1 && state[0].line.isEmpty()) {
        state.removeAt(0);
    } else {
        sourceChanged = !filetype().isEmpty() && !change.delayable();
    }
    emit inProgressChanged(1);
    emit changeMerged(state, source, sourceChanged);
}

// TODO: set cursorPosition of the state to the end of last Replacement
// potentially needing to record a second counter
void BufferWorker::replace(BufferState &state, const QList<Replacement> &replaces)
{
    if (state.empty() || replaces.empty())
        return;
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
    highlight(state, stateIndex, highlightState, oldHighlightState);
    return;
}
