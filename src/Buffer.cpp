/*
 * Buffer.cpp
 *
 *  Created on: Nov 23, 2014
 *      Author: lingnan
 */

#include <QTextStream>
#include <src/Buffer.h>
#include <src/srchilite/instances.h>
#include <src/srchilite/langmap.h>
#include <src/SaveWork.h>
#include <QDebug>

#define SECONDS_TO_REGISTER_HISTORY 1
#define DEFAULT_EDIT_TIME (QDateTime::fromTime_t(0))

Buffer::Buffer(int historyLimit) :
    _langMap(srchilite::Instances::getLangMap()),
    _emittingStateChange(false),
    _sourceHighlight("default.style", "xhtml.outlang"),
    _states(historyLimit),
    _lastEdited(DEFAULT_EDIT_TIME),
    _worker(this)
{
    conn(&_worker, SIGNAL(inProgressChanged(float)),
        this, SIGNAL(inProgressChanged(float)));
//    conn(&_states, SIGNAL(retractableChanged(bool)),
//        this, SIGNAL(hasUndosChanged(bool)));
//    conn(&_states, SIGNAL(advanceableChanged(bool)),
//        this, SIGNAL(hasRedosChanged(bool)));
}

Buffer::~Buffer()
{
    _worker.quit();
    _worker.wait();
}

const QString &Buffer::name() const { return _name; }

void Buffer::setName(const QString& name)
{
    if (name != _name) {
        _name = name;
        // try to set the filetype
        std::string ft = name.toUtf8().constData();
        ft = _langMap->getMappedFileNameFromFileName(ft);
        setFiletype(QString::fromUtf8(ft.c_str()));
        emit nameChanged(name);
    }
}

const QString &Buffer::filetype() { return _filetype; }

void Buffer::setFiletype(const QString &filetype)
{
    if (filetype != _filetype) {
        _filetype = filetype;
        qDebug() << "Setting filetype to:" << _filetype;
        if (_filetype.isEmpty()) {
            _mainStateData.reset();
        } else {
            _sourceHighlight.setInputLang(std::string(_filetype.toUtf8().constData()) + ".lang");
            // initialize the main state data
            _mainStateData = HighlightStateDataPtr(new HighlightStateData(
                    _sourceHighlight.getHighlighter()->getMainState(),
                    srchilite::HighlightStateStackPtr(new srchilite::HighlightStateStack())
            ));
        }
        highlight(state());
        emit filetypeChanged(_filetype);
    }
}

BufferState &Buffer::state() { return _states.current(); }

bool Buffer::emittingStateChange() const { return _emittingStateChange; }

void Buffer::emitStateChange(View *source, bool sourceChanged, bool shouldMatchCursorPosition)
{
    _emittingStateChange = true;
    emit stateChanged(state(), source, sourceChanged, shouldMatchCursorPosition);
    _emittingStateChange = false;
}

void Buffer::highlight(BufferState &state)
{
    state.setFiletype(filetype());
    HighlightStateDataPtr currentHighlightData = _mainStateData;
    for (int i = 0; i < state.size(); i++) {
        currentHighlightData = highlightLine(state[i], currentHighlightData);
    }
}

HighlightStateDataPtr Buffer::highlightLine(BufferLine &line, HighlightStateDataPtr startState)
{
    if (filetype().isEmpty() || line.isEmpty() || !startState) {
        line.setHighlightText(QString());
        line.setEndHighlightState(startState);
    } else {
        HighlightStateDataPtr state = HighlightStateDataPtr(new HighlightStateData(*startState));
        _sourceHighlight.getHighlighter()->setCurrentState(state->currentState);
        _sourceHighlight.getHighlighter()->setStateStack(state->stateStack);
        _sourceHighlight.clearBuffer();
        QByteArray plainText;
        QTextStream stream(&plainText);
        line.writePlainText(stream);
        stream << flush;
        _sourceHighlight.getHighlighter()->highlightParagraph(std::string(plainText.constData()));
        line.setHighlightText(QString::fromUtf8(_sourceHighlight.getBuffer().str().c_str()));
        state->currentState = _sourceHighlight.getHighlighter()->getCurrentState();
        line.setEndHighlightState(state);
    }
    return line.endHighlightState();
}

// assumption: input does contain some change
// the state was in sync before the change in the input (with the correct filetype and all)
// returning: have we highlighted any change?
bool Buffer::mergeChange(BufferState &state, QTextStream &input, int cursorPosition, bool enableDelay)
{
    state.setCursorPosition(cursorPosition);
    BufferStateChange change = _bufferChangeParser.parseBufferChange(input, cursorPosition);
    qDebug() << "change:" << change;
    // pull in the changes
    int bufferIndex = qMax(change.startIndex(), 0);
    int changeIndex = -1;
    int offset = 0;
    HighlightStateDataPtr currentHighlightData;
    int beforeStartIndex = bufferIndex - 1;
    if (beforeStartIndex >= 0 && beforeStartIndex < state.size()) {
        currentHighlightData = state[beforeStartIndex].endHighlightState();
    } else {
        currentHighlightData = _mainStateData;
    }
    HighlightStateDataPtr lastEndHighlightData = currentHighlightData;
    for (int i = 0; i < change.size(); i++) {
        currentHighlightData = highlightLine(change[i].line, currentHighlightData);
        changeIndex = change[i].startIndex;
        if (changeIndex < 0) {
            // we need to insert this new line
            state.insert(bufferIndex, change[i].line);
            offset++;
        } else {
            changeIndex = change[i].endIndex;
            // we need to delete the lines until the index match up
            while (bufferIndex != changeIndex+offset) {
                state.removeAt(bufferIndex);
                offset--;
            }
            lastEndHighlightData = state[bufferIndex].endHighlightState();
            state[bufferIndex] = change[i].line;
        }
        bufferIndex++;
    }
    // if there is no merge edge then we can remove all the rest
    if (changeIndex < 0) {
        while (bufferIndex < state.size()) {
            state.removeLast();
        }
    } else {
        qDebug() << "continuing to highlight additional lines";
        while (bufferIndex < state.size() &&
                lastEndHighlightData != currentHighlightData && *lastEndHighlightData != *currentHighlightData){
            qDebug() << "highlighting line" << bufferIndex;
            lastEndHighlightData = state[bufferIndex].endHighlightState();
            currentHighlightData = highlightLine(state[bufferIndex], currentHighlightData);
            bufferIndex++;
        }
    }
    // if there is nothing left.
    // there can be no highlight change when the buffer is empty
    if (state.size() == 1 && state[0].isEmpty()) {
        state.removeAt(0);
        return false;
    }
    return !filetype().isEmpty() && (!enableDelay || !change.delayable());
}

QDebug operator<<(QDebug dbg, const BufferLine *line)
{
    if (!line) {
        dbg << "<nil>";
    } else {
        dbg << *line;
    }
    return dbg;
}

void Buffer::replace(BufferState &state, const QList<Replacement> &replaces)
{
    if (state.empty() || replaces.empty())
        return;
    int charCount = 0;
    int stateIndex = 0;
    bool shouldHighlight = false;
    BufferLine temp; // used as a temp storage for pointers
    BufferLine *current = &state[stateIndex], *before = NULL, *after = NULL;
    HighlightStateDataPtr highlightState = _mainStateData;
    for (int i = 0; i < replaces.size(); i++) {
        const Replacement &rep = replaces[i];
        qDebug() << "replacing" << rep;
        // fast forward until reaching the replace start
        while (true) {
            qDebug() << "charCount:" << charCount << "stateIndex:" << stateIndex;
            qDebug() << "current:" << current;
            qDebug() << "before:" << before;
            qDebug() << "after:" << after;
            if (charCount + current->size() < rep.selection.start) {
                qDebug() << "skippinng the current line";
                charCount += current->size() + 1;
            } else {
                qDebug() << "splitting the current line";
                temp = current->split(rep.selection.start - charCount);
                qDebug() << "updating the charCount to match the start of selection";
                charCount = rep.selection.start;
                after = &temp;
                qDebug() << "current:" << current;
                qDebug() << "after:" << after;
            }
            if (before) {
                qDebug() << "appending current to before";
                before->append(*current);
                current = before;
                before = NULL;
                qDebug() << "current:" << current;
                qDebug() << "before:" << before;
            }
            if (after) {
                shouldHighlight = !filetype().isEmpty();
                qDebug() << "setting shouldHighlight to" << shouldHighlight;
                break;
            }
            if (shouldHighlight) {
                HighlightStateDataPtr oldEnd = current->endHighlightState();
                qDebug() << "highlighting current";
                highlightState = highlightLine(*current, highlightState);
                if (oldEnd == highlightState || (oldEnd && highlightState && *oldEnd == *highlightState))
                    shouldHighlight = false;
            } else {
                qDebug() << "setting highlight state to the end state of current";
                highlightState = current->endHighlightState();
            }
            qDebug() << "advancing the stateIndex and pointing current to the next line in state";
            current = &state[++stateIndex];
        }
        // start inserting the replaced content
        qDebug() << "inserting replacement into state...";
        for (int j = 0; j < rep.replacement.size(); j++) {
            QChar ch = rep.replacement[j];
            if (ch == '\n') {
                highlightState = highlightLine(*current, highlightState);
                state.insert(++stateIndex, BufferLine());
                qDebug() << "newline met: advancing stateIndex to" << stateIndex;
                qDebug() << "inserting a new BufferLine at the new stateIndex and pointing current to it";
                current = &state[stateIndex];
            } else {
                current->append(ch);
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
        while (charCount + current->size() < rep.selection.end) {
            qDebug() << "charCount:" << charCount;
            qDebug() << "skippinng the current line";
            charCount += current->size() + 1;
            temp = state[stateIndex+1];
            qDebug() << "detaching from index:" << stateIndex+1 << "line:" << temp;
            qDebug() << "removing line at index:" << stateIndex+1;
            state.removeAt(stateIndex+1);
            qDebug() << "pointing current to the detached line";
            current = &temp;
        }
        qDebug() << "charCount:" << charCount;
        // split again to remove the replaced part
        temp = current->split(rep.selection.end - charCount);
        qDebug() << "splitting again to remove the final part of the selection, split:" << temp;
        qDebug() << "pointing current to split";
        current = &temp;
    }
    qDebug() << "finished all the replacements";
    qDebug() << "current:" << current;
    qDebug() << "before:" << before;
    qDebug() << "appending current to the end of before";
    before->append(*current);
    if (shouldHighlight) {
        qDebug() << "continuing to highlight";
        qDebug() << "pointing current to before";
        current = before;
        while (true) {
            HighlightStateDataPtr oldEnd = current->endHighlightState();
            qDebug() << "highlighting line" << stateIndex;
            highlightState = highlightLine(*current, highlightState);
            if (oldEnd == highlightState || oldEnd && highlightState && *oldEnd == *highlightState)
                break;
            if (++stateIndex >= state.size())
                break;
            current = &state[stateIndex];
        }
    }
    return;
}

BufferState &Buffer::modifyState()
{
    // save history state
    QDateTime current = QDateTime::currentDateTime();
    if (current >= _lastEdited.addSecs(SECONDS_TO_REGISTER_HISTORY)) {
        _states.copyCurrent();
    }
    _lastEdited = current;
    return state();
}

// TODO: also tackle the case where the editor is moved, selection selected
// in other words, in such situations you also need to rehighlight any delayed content
void Buffer::parseChange(View *source, const QString &content, int cursorPosition, bool enableDelay)
{

    QString cont(content);
    QTextStream input(&cont);
    emitStateChange(source, mergeChange(modifyState(), input, cursorPosition, enableDelay), false);
}

void Buffer::parseReplacement(View *source, QList<Replacement> &replaces)
{
    if (!replaces.empty()) {
        replace(modifyState(), replaces);
        emitStateChange(source, true, false);
    }
}

bool Buffer::hasUndo() { return _states.retractable(); }

bool Buffer::hasRedo() { return _states.advanceable(); }

void Buffer::undo()
{
    if (_states.retract()) {
        if (state().filetype() != filetype()) {
            highlight(state());
        }
        _lastEdited = DEFAULT_EDIT_TIME;
        emitStateChange(NULL, true, true);
    }
}

void Buffer::redo()
{
    if (_states.advance()) {
        if (state().filetype() != filetype()) {
            highlight(state());
        }
        _lastEdited = DEFAULT_EDIT_TIME;
        emitStateChange(NULL, true, true);
    }
}

/* file related operations */
void Buffer::save()
{
    if (!_worker.isRunning()) {
        BufferWorkPtr w = _worker.work();
        if (!w || w->type() != Save) {
            w = BufferWorkPtr(new SaveWork);
            _worker.setWork(w);
        }
        boost::dynamic_pointer_cast<SaveWork>(w)->setState(state());
        _worker.start();
    } else if (_worker.work()->type() != Save) {
        qDebug() << "save action activated while some other job" << _worker.work()->type() << "is running!";
    }
}
