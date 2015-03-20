/*
 * Buffer.cpp
 *
 *  Created on: Nov 23, 2014
 *      Author: lingnan
 */

#include <QTextStream>
#include <src/Buffer.h>
#include <stdio.h>
#include <src/srchilite/instances.h>
#include <src/srchilite/langmap.h>
#include <src/HtmlPlainTextExtractor.h>
#include <src/SaveWork.h>

#define SECONDS_TO_REGISTER_HISTORY 1
#define DEFAULT_EDIT_TIME (QDateTime::fromTime_t(0))

Buffer::Buffer(int historyLimit) :
    _langMap(srchilite::Instances::getLangMap()),
    _emittingContentChange(false),
    _sourceHighlight(std::string(style.toUtf8().constData()), "xhtml.outlang"),
    _states(BufferHistory(historyLimit)),
    _lastEdited(DEFAULT_EDIT_TIME),
    _worker(this)
{
    conn(&_worker, SIGNAL(inProgressChanged(float)),
        this, SIGNAL(inProgressChanged(float)));
    conn(&_states, SIGNAL(retractableChanged(bool)),
        this, SIGNAL(hasUndosChanged(bool)));
    conn(&_states, SIGNAL(advanceableChanged(bool)),
        this, SIGNAL(hasRedosChanged(bool)));
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

const QString &Buffer::filetype() const { return _filetype; }

void Buffer::setFiletype(const QString &filetype)
{
    if (filetype != _filetype) {
        _filetype = filetype;
        printf("Setting filetype to: %s\n", qPrintable(_filetype));
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
        highlight(_states.current());
        emit filetypeChanged(_filetype);
    }
}

const BufferState &Buffer::state() const { return _states.current(); }

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
    if (filetype().isEmpty()) {
        line.setHighlightText(QString());
        line.endHighlightState().reset();
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
    // pull in the changes
    int bufferIndex = qMax(change.startIndex(), 0);
    int changeIndex = 0;
    int offset = 0;
    HighlightStateDataPtr currentHighlightData;
    int beforeStartIndex = bufferIndex - 1;
    if (beforeStartIndex >= 0 && beforeStartIndex < state.size()) {
        currentHighlightData = state[beforeStartIndex].endHighlightState();
    } else {
        currentHighlightData = _mainStateData;
    }
    HighlightStateDataPtr lastEndHighlightData = currentHighlightData;
    while (changeIndex < change.size()) {
        ChangedBufferLine &ch = change[changeIndex];
        currentHighlightData = highlightLine(ch.line, currentHighlightData);
        if (ch.index < 0) {
            // we need to insert this new line
            state.insert(bufferIndex, ch.line);
            offset++;
        } else {
            // we need to delete the lines until the index match up
            while (bufferIndex != ch.index+offset) {
                state.removeAt(bufferIndex);
                offset--;
            }
            lastEndHighlightData = state[bufferIndex].endHighlightState();
            state[bufferIndex] = ch.line;
        }
        changeIndex++;
        bufferIndex++;
    }
    while (bufferIndex < state.size() && *lastEndHighlightData != *currentHighlightData){
        lastEndHighlightData = state[bufferIndex].endHighlightState();
        currentHighlightData = highlightLine(state[bufferIndex], currentHighlightData);
        bufferIndex++;
    }
    return !filetype().isEmpty() && !enableDelay || !change.delayable();
}

void Buffer::replace(BufferState &state, const QList<QPair<TextSelection, QString> > &replaces)
{
    // a replace buffer recording currently the stuff to be inserted
    // a counter recording how many more (consecutive) characters to be replaced
    // this allows us to break replace position and strings into pieces
    // an offset that tells us how many more characters to eat away before the
    // next replace starts (this is to account for the fact that these text selections
    // are all relative to the old buffer
}

BufferState &Buffer::modifyState()
{
    // save history state
    QDateTime current = QDateTime::currentDateTime();
    if (current >= _lastEdited.addSecs(SECONDS_TO_REGISTER_HISTORY)) {
        _states.copyCurrent();
    }
    _lastEdited = current;
    return _states.current();
}

// TODO: also tackle the case where the editor is moved, selection selected
// in other words, in such situations you also need to rehighlight any delayed content
void Buffer::parseChange(View *source, const QString &content, int cursorPosition, bool enableDelay)
{

    QString cont(content);
    QTextStream input(&cont);
    emitStateChange(source, mergeChange(modifyState(), input, cursorPosition, enableDelay), false);
}

void Buffer::parseReplacement(View *source, QList<QPair<TextSelection, QString> > &replaces)
{
    if (replaces.count() == 0)
        return;
    bool sourceChanged = false;
    QString cont(_content);
    QTextStream input(&cont);
    _content.clear();
    QTextStream output(&_content);
    if (filetype().isEmpty()) {
        sourceChanged = _extractor.replacePlainText(input, output, replaces);
    } else {
        sourceChanged = _highlight.replaceHtml(input, output, replaces);
    }
    output << flush;
//        registerContentChange(-1);
    emitContentChanged(source, sourceChanged, -1);
}

bool Buffer::hasUndo() const { return _states.retractable(); }

bool Buffer::hasRedo() const { return _states.advanceable(); }

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
        boost::dynamic_pointer_cast<SaveWork>(w)->setHtml(content());
        _worker.start();
    } else if (_worker.work()->type() != Save) {
        printf("save action activated while some other job %d is running!\n", _worker.work()->type());
    }
}

bool Buffer::hasPlainText()
{
    QTextStream input(&_content);
    return _extractor.hasPlainText(input);
}

QString Buffer::plainText()
{
    QTextStream input(&_content);
    QString cont;
    QTextStream output(&cont);
    _extractor.extractPlainText(input, output);
    output << flush;
    return cont;
}
