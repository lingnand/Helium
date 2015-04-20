/*
 * Buffer.cpp
 *
 *  Created on: Nov 23, 2014
 *      Author: lingnan
 */

#include <QTextStream>
#include <src/Buffer.h>
#include <src/SaveWork.h>
#include <QDebug>

#define SECONDS_TO_REGISTER_HISTORY 1
#define DEFAULT_EDIT_TIME (QDateTime::fromTime_t(0))

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

// Buffer
Buffer::Buffer(int historyLimit):
    _requestId(0),
    _states(historyLimit),
    _lastEdited(DEFAULT_EDIT_TIME),
    _locked(false)
{
    _worker.moveToThread(&_workerThread);
    conn(this, SIGNAL(workerInitialize()),
            &_worker, SLOT(initialize()));
    conn(this, SIGNAL(workerSetFiletype(unsigned int, BufferState &, const QString &)),
            &_worker, SLOT(setFiletype(unsigned int, BufferState &, const QString &)));
    conn(this, SIGNAL(workerSaveStateToFile(const BufferState &, const QString &)),
            &_worker, SLOT(saveStateToFile(const BufferState &, const QString &)));
    conn(this, SIGNAL(workerLoadStateFromFile(const QString &)),
            &_worker, SLOT(loadStateFromFile(const QString &)));
    conn(this, SIGNAL(workerMergeChange(unsigned int, BufferState &, View *, const BufferStateChange &)),
            &_worker, SLOT(mergeChange(unsigned int, BufferState &, View *, const BufferStateChange &)));
    conn(this, SIGNAL(workerReplace(unsigned int, BufferState &, const QList<Replacement> &)),
            &_worker, SLOT(replace(unsigned int, BufferState &, const QList<Replacement> &)));
    conn(this, SIGNAL(workerRehighlight(unsigned int, BufferState &, View *, int)),
            &_worker, SLOT(rehighlight(unsigned int, BufferState &, View *, int)));

    conn(&_worker, SIGNAL(inProgressChanged(float)),
            this, SIGNAL(inProgressChanged(float)));
    conn(&_worker, SIGNAL(noUpdate(unsigned int)),
            this, SLOT(onWorkerNoUpdate(unsigned int)));
    conn(&_worker, SIGNAL(stateLoadedFromFile(const BufferState &, const QString &)),
            this, SLOT(onWorkerStateLoadedFromFile(const BufferState &, const QString &)));
    conn(&_worker, SIGNAL(stateUpdated(unsigned int, const BufferState &, View *, bool)),
            this, SLOT(onWorkerStateUpdated(unsigned int, const BufferState &, View *, bool)));
    _workerThread.start();
    // initialize in the background
    emit workerInitialize();

    conn(&_states, SIGNAL(retractableChanged(bool)),
            this, SIGNAL(hasUndosChanged(bool)));
    conn(&_states, SIGNAL(advanceableChanged(bool)),
            this, SIGNAL(hasRedosChanged(bool)));
}

Buffer::~Buffer()
{
    _workerThread.quit();
    _workerThread.wait();
}

bool Buffer::locked() const { return _locked; }

void Buffer::setLocked(bool lock) {
    if (_locked != lock) {
        _locked = lock;
        emit lockedChanged(lock);
    }
}

const QString &Buffer::name() const { return _name; }

void Buffer::setName(const QString& name)
{
    if (name != _name) {
        _name = name;
        setFiletype(_worker.filetypeForName(name));
        emit nameChanged(name);
    }
}

const QString &Buffer::filetype() { return _worker.filetype(); }

void Buffer::setFiletype(const QString &filetype)
{
    QString oldft = _worker.filetype();
    if (filetype == oldft)
        return;
    BufferState &st = state();
    if (!st.isEmpty()) {
        // if state is empty we don't need to lock the textArea -- really
        setLocked(true);
    }
    emit workerSetFiletype(++_requestId, state(), filetype);
    emit filetypeChanged(filetype);
}

BufferState &Buffer::state() { return _states.current(); }

void Buffer::onWorkerNoUpdate(unsigned int requestId)
{
    if (_requestId > requestId)
        return;
    setLocked(false);
}

// assumption: input does contain some change
// the state was in sync before the change in the input (with the correct filetype and all)
// returning: have we highlighted any change?
void Buffer::onWorkerStateUpdated(unsigned int requestId, const BufferState &newSt, View *source, bool shouldUpdateSourceView)
{
    if (_requestId > requestId) {
        qDebug() << "got result for" << requestId << ", but expecting" << _requestId;
        return;
    }
    qDebug() << "got new state";
    BufferState &st = state();
    if (&st != &newSt) {
        qDebug() << "a new state is copied into our list!";
        st = newSt;
    }
    setLocked(false);
    emit stateChanged(st, source, shouldUpdateSourceView);
}

void Buffer::onWorkerStateLoadedFromFile(const BufferState &st, const QString &filename)
{
    // TODO: set the filename before here
    // (and without rehighlighting the current buffer)
    // (make sure highlight related settings are only tampered with in the main thread)
    // TODO: should we clear out the entire state list?
    state() = st;
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
void Buffer::parseChange(View *source, const QString &content, ParserPosition start, int cursorPosition)
{

    BufferState &state = modifyState();
    state.setCursorPosition(cursorPosition);
    BufferStateChange change = _bufferChangeParser.parseBufferChange(content, start, cursorPosition);
    qDebug() << "change:" << change;
    if (change.size() > 100) { // put this into background
        setLocked(true);
        emit workerMergeChange(++_requestId, state, source, change);
    } else {
        _worker.mergeChange(++_requestId, state, source, change);
    }
}

void Buffer::parseReplacement(const Replacement &replace)
{
    parseReplacement(QList<Replacement>() << replace);
}

void Buffer::parseReplacement(const QList<Replacement> &replaces)
{
    if (!replaces.empty() && !state().isEmpty()) {
        BufferState &state = modifyState();
        if (replaces.size() > 20) { // put this into background
            setLocked(true);
            emit workerReplace(++_requestId, state, replaces);
        } else {
            _worker.replace(++_requestId, state, replaces);
        }
    }
}

// deleting until the start of the specified line and
void Buffer::killLine(View *source, int cursorPosition)
{
    BufferState::Position pos = state().focus(cursorPosition);
    if (!state()[pos.lineIndex].line.isEmpty()) {
        BufferState &state = modifyState();
        state.setCursorPosition(cursorPosition - pos.linePosition);
        state[pos.lineIndex].line.clear();
        // blocking rehighlight (we assume the chagne is small)
        _worker.rehighlight(++_requestId, state, source);
    }
}

bool Buffer::hasUndo() { return _states.retractable(); }

bool Buffer::hasRedo() { return _states.advanceable(); }

void Buffer::undo()
{
    if (_states.retract()) {
        BufferState &st = state();
        _lastEdited = DEFAULT_EDIT_TIME;
        if (st.filetype() != filetype()) {
            // rehighlight in the background
            setLocked(true);
            emit workerRehighlight(++_requestId, st);
        } else {
            // directly emit a state change
            emit stateChanged(st, NULL, true);
        }
    }
}

void Buffer::redo()
{
    if (_states.advance()) {
        BufferState &st = state();
        _lastEdited = DEFAULT_EDIT_TIME;
        if (st.filetype() != filetype()) {
            setLocked(true);
            emit workerRehighlight(++_requestId, st);
        } else {
            emit stateChanged(st, NULL, true);
        }
    }
}

/* file related operations */
void Buffer::save()
{
    emit workerSaveStateToFile(state(), "");
}
