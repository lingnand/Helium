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
    _langMap(srchilite::Instances::getLangMap()),
    _states(historyLimit),
    _lastEdited(DEFAULT_EDIT_TIME),
    _locked(false)
{
    _worker.moveToThread(&_workerThread);
    conn(this, SIGNAL(workerSaveStateToFile(const BufferState &, const QString &)),
            &_worker, SLOT(saveStateToFile(const BufferState &, const QString &)));
    conn(this, SIGNAL(workerLoadStateFromFile(const QString &)),
            &_worker, SLOT(loadStateFromFile(const QString &)));
    conn(this, SIGNAL(workerMergeChange(BufferState &, View *, const BufferStateChange &)),
            &_worker, SLOT(mergeChange(BufferState &, View *, const BufferStateChange &)));

    conn(&_worker, SIGNAL(inProgressChanged(float)),
            this, SIGNAL(inProgressChanged(float)));
    conn(&_worker, SIGNAL(stateLoadedFromFile(const BufferState &, const QString &)),
            this, SLOT(onWorkerStateLoadedFromFile(const BufferState &, const QString &)));
    conn(&_worker, SIGNAL(changeMerged(const BufferState &, View *, bool)),
            this, SLOT(onWorkerChangeMerged(const BufferState &, View *, bool)));
    _workerThread.start();

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
        // try to set the filetype
        std::string ft = name.toUtf8().constData();
        ft = _langMap->getMappedFileNameFromFileName(ft);
        setFiletype(QString::fromUtf8(ft.c_str()));
        emit nameChanged(name);
    }
}

const QString &Buffer::filetype() { return _worker.filetype(); }

void Buffer::setFiletype(const QString &filetype)
{
    if (_worker.setFiletype(filetype)) {
        BufferState &st = state();
        _worker.highlight(st);
        if (!st.isEmpty()) {
            emit stateChanged(st, NULL, true, false);
        }
        emit filetypeChanged(filetype);
    }
}

BufferState &Buffer::state() { return _states.current(); }

// assumption: input does contain some change
// the state was in sync before the change in the input (with the correct filetype and all)
// returning: have we highlighted any change?
void Buffer::onWorkerChangeMerged(const BufferState &newSt, View *source, bool shouldUpdateSourceView)
{
    BufferState &st = state();
    if (&st != &newSt) {
        qDebug() << "change merged and a new state is copied into our list!";
        st = newSt;
    }
    setLocked(false);
    emit stateChanged(st, source, shouldUpdateSourceView, false);
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
    if (change.size() > 100) {
        // put this into background
        setLocked(true);
        emit workerMergeChange(state, source, change);
    } else {
        _worker.mergeChange(state, source, change);
    }
}

void Buffer::parseReplacement(View *source, const Replacement &replace)
{
    parseReplacement(source, QList<Replacement>() << replace);
}

void Buffer::parseReplacement(View *source, const QList<Replacement> &replaces)
{
    if (!replaces.empty()) {
        BufferState &state = modifyState();
        _worker.replace(state, replaces);
        emit stateChanged(state, source, true, false);
    }
}

// deleting until the start of the specified line and
void Buffer::killLine(int index)
{
    if (!state()[index].line.isEmpty()) {
        BufferState &state = modifyState();
        state[index].line.clear();
        _worker.highlight(state, index);
        emit stateChanged(state, NULL, true, false);
    }
}

bool Buffer::hasUndo() { return _states.retractable(); }

bool Buffer::hasRedo() { return _states.advanceable(); }

void Buffer::undo()
{
    if (_states.retract()) {
        BufferState &st = state();
        if (st.filetype() != filetype()) {
            _worker.highlight(st);
        }
        _lastEdited = DEFAULT_EDIT_TIME;
        emit stateChanged(st, NULL, true, true);
    }
}

void Buffer::redo()
{
    if (_states.advance()) {
        BufferState &st = state();
        if (st.filetype() != filetype()) {
            _worker.highlight(st);
        }
        _lastEdited = DEFAULT_EDIT_TIME;
        emit stateChanged(st, NULL, true, true);
    }
}

/* file related operations */
void Buffer::save()
{
    emit workerSaveStateToFile(state(), "");
}
