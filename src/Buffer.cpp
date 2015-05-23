/*
 * Buffer.cpp
 *
 *  Created on: Nov 23, 2014
 *      Author: lingnan
 */

#include <QFileInfo>
#include <QTextStream>
#include <QDebug>
#include <Buffer.h>
#include <Helium.h>
#include <SignalBlocker.h>
#include <Filetype.h>
#include <FiletypeMap.h>
#include <Utility.h>

#define SECONDS_TO_REGISTER_HISTORY 1
#define DEFAULT_EDIT_TIME (QDateTime::fromTime_t(0))

// Buffer
Buffer::Buffer(int historyLimit, QObject *parent):
    QObject(parent),
    _requestId(0),
    _states(historyLimit),
    _lastEdited(DEFAULT_EDIT_TIME),
    _locked(false), _dirty(false), _autodetectFiletype(true)
{
    _worker.moveToThread(&_workerThread);
    conn(this, SIGNAL(workerSetFiletype(StateChangeContext&, BufferState&, Filetype*, Progress&)),
         &_worker, SLOT(setFiletype(StateChangeContext&, BufferState&, Filetype*, Progress&)));
    conn(this, SIGNAL(workerParseAndMergeChange(StateChangeContext&, BufferState&, const QString&, ParserPosition, int, Progress&)),
         &_worker, SLOT(parseAndMergeChange(StateChangeContext&, BufferState&, const QString&, ParserPosition, int, Progress&)));
    conn(this, SIGNAL(workerMergeChange(StateChangeContext&, BufferState&, const BufferStateChange&, Progress&)),
         &_worker, SLOT(mergeChange(StateChangeContext&, BufferState&, const BufferStateChange&, Progress&)));
    conn(this, SIGNAL(workerReplace(StateChangeContext&, BufferState&, const QList<Replacement>&, Progress&)),
         &_worker, SLOT(replace(StateChangeContext&, BufferState&, const QList<Replacement>&, Progress&)));
    conn(this, SIGNAL(workerRehighlight(StateChangeContext&, BufferState&, int, Progress&)),
         &_worker, SLOT(rehighlight(StateChangeContext&, BufferState&, int, Progress&)));
    conn(this, SIGNAL(workerSaveStateToFile(const BufferState&, const QString&, Progress&)),
         &_worker, SLOT(saveStateToFile(const BufferState&, const QString&, Progress&)));
    conn(this, SIGNAL(workerLoadStateFromFile(StateChangeContext&, const QString&, bool, Progress&)),
         &_worker, SLOT(loadStateFromFile(StateChangeContext&, const QString&, bool, Progress&)));

    conn(&_worker, SIGNAL(progressChanged(float, bb::cascades::ProgressIndicatorState::Type, const QString&)),
            this, SIGNAL(progressChanged(float, bb::cascades::ProgressIndicatorState::Type, const QString&)));
    conn(&_worker, SIGNAL(filetypeChanged(const StateChangeContext&, const BufferState&)),
            this, SLOT(handleStateChangeResult(const StateChangeContext&, const BufferState&)));
    conn(&_worker, SIGNAL(changeMerged(const StateChangeContext&, const BufferState&)),
            this, SLOT(handleStateChangeResult(const StateChangeContext&, const BufferState&)));
    conn(&_worker, SIGNAL(occurrenceReplaced(const StateChangeContext&, const BufferState&)),
            this, SLOT(handleStateChangeResult(const StateChangeContext&, const BufferState&)));
    conn(&_worker, SIGNAL(stateRehighlighted(const StateChangeContext&, const BufferState&)),
            this, SLOT(handleStateChangeResult(const StateChangeContext&, const BufferState&)));
    conn(&_worker, SIGNAL(stateLoadedFromFile(const StateChangeContext&, const BufferState&, const QString&)),
            this, SLOT(handleStateChangeResult(const StateChangeContext&, const BufferState&)));
    // report
    conn(&_worker, SIGNAL(stateSavedToFile(const QString&)),
            this, SIGNAL(savedToFile(const QString&)));
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

void Buffer::setLocked(bool lock) {
    if (_locked != lock) {
        _locked = lock;
        emit lockedChanged(lock);
    }
}

void Buffer::setDirty(bool dirty) {
    if (_dirty != dirty) {
        _dirty = dirty;
        emit dirtyChanged(dirty);
    }
}

void Buffer::setName(const QString &name)
{
    Progress progress;
    _setName(name, _autodetectFiletype, progress);
}

void Buffer::_setName(const QString &name, bool setft, Progress &progress)
{
    if (name != _name) {
        _name = name;
        if (setft)
            _setFiletype(Helium::instance()->filetypeMap()->filetypeForName(name), progress);
        emit nameChanged(name);
    }
}

void Buffer::setFilepath(const QString &filepath, bool setFiletype, Progress &progress)
{
    if (filepath != _filepath) {
        _filepath = filepath;
        _setName(QFileInfo(_filepath).fileName(), setFiletype, progress);
        emit filepathChanged(_filepath);
    }
}

void Buffer::setFiletype(Filetype *filetype)
{
    Progress progress;
    _setFiletype(filetype, progress);
}

void Buffer::_setFiletype(Filetype *filetype, Progress &progress)
{
    BufferState &st = _states.current();
    if (filetype != st.filetype()) {
        StateChangeContext ctx(++_requestId);
        if (!st.isEmpty()) {
            // if state is empty we don't need to lock the textArea -- really
            setLocked(true);
        }
        emit workerSetFiletype(ctx, st, filetype, progress);
        progress.current = progress.cap;
    }
}

void Buffer::setAutodetectFiletype(bool autodetect)
{
    if (autodetect != _autodetectFiletype) {
        _autodetectFiletype = autodetect;
        emit autodetectFiletypeChanged(_autodetectFiletype);
    }
}

// assumption: this should handle result from another thread
void Buffer::handleStateChangeResult(const StateChangeContext &ctx, const BufferState &newSt)
{
    if (_requestId > ctx.requestId) {
        qDebug() << "got result for" << ctx.requestId << ", but expecting" << _requestId;
        return;
    }
    if (newSt.filetype() != filetype()) {
        emit filetypeChanged(newSt.filetype(), filetype());
    }
    _states.current() = newSt;
    setLocked(false);
    emit stateChanged(ctx, newSt);
}

BufferState &Buffer::modifyState()
{
    // save history state
    QDateTime current = QDateTime::currentDateTime();
    if (current >= _lastEdited.addSecs(SECONDS_TO_REGISTER_HISTORY)) {
        _states.copyCurrent();
    }
    _lastEdited = current;
    setDirty(true);
    return _states.current();
}

// TODO: also tackle the case where the editor is moved, selection selected
// in other words, in such situations you also need to rehighlight any delayed content
void Buffer::parseChange(View *source, const QString &content, ParserPosition start, int cursorPosition)
{
    StateChangeContext ctx(++_requestId, source);
    BufferState &state = modifyState();
    if (state.filetype() && state.filetype()->highlightEnabled()) {
        BufferStateChange change = _worker.parseBufferChange(state, content, start, cursorPosition);
        qDebug() << "change:" << change;
        qDebug() << "changeSize:" << change.size();
        Progress progress;
        if (change.size() > 100) { // put this into background
            setLocked(true);
            emit workerMergeChange(ctx, state, change, progress);
        } else {
            SignalBlocker blocker(&_worker);
            _worker.mergeChange(ctx, state, change, progress);
            emit stateChanged(ctx, state);
        }
    } else {
        // for empty filetype we can process the change in the background
        // without locking because each change is considered complete
        // furthermore, because of completeness we can throw away the
        // current state completely
        BufferState empty;
        Progress progress(0, 0);
        emit workerParseAndMergeChange(ctx, empty, content, start, cursorPosition, progress);
    }
}

void Buffer::parseReplacement(const Replacement &replace)
{
    parseReplacement(QList<Replacement>() << replace);
}

void Buffer::parseReplacement(const QList<Replacement> &replaces)
{
    if (!replaces.empty() && !state().isEmpty()) {
        StateChangeContext ctx(++_requestId);
        BufferState &state = modifyState();
        Progress progress;
        if (replaces.size() > 20) { // put this into background
            setLocked(true);
            emit workerReplace(ctx, state, replaces, progress);
        } else {
            SignalBlocker blocker(&_worker);
            _worker.replace(ctx, state, replaces, progress);
            emit stateChanged(ctx, state);
        }
    }
}

// deleting until the start of the specified line and
void Buffer::killLine(View *source, int cursorPosition)
{
    BufferState::Position pos = state().focus(cursorPosition);
    qDebug() << "killing line with position:" << pos;
    if (pos.lineIndex >= 0 && !state()[pos.lineIndex].line.isEmpty()) {
        StateChangeContext ctx(++_requestId, source);
        BufferState &state = modifyState();
        state.setCursorPosition(cursorPosition - pos.linePosition);
        state[pos.lineIndex].line.clear();
        // blocking rehighlight (we assume the change is small)
        Progress progress;
        SignalBlocker blocker(&_worker);
        _worker.rehighlight(ctx, state, pos.lineIndex, progress);
        emit stateChanged(ctx, state);
    }
}

void Buffer::undo()
{
    traverse(&BufferHistory::retract);
}

void Buffer::redo()
{
    traverse(&BufferHistory::advance);
}

void Buffer::traverse(bool (BufferHistory::*fn)())
{
    Filetype *ft = filetype();
    if ((_states.*fn)()) {
        StateChangeContext ctx(++_requestId, NULL, true, true);
        setDirty(true);
        BufferState &st = _states.current();
        _lastEdited = DEFAULT_EDIT_TIME;
        if (st.filetype() != ft) {
            // rehighlight in the background
            setLocked(true);
            Progress progress;
            emit workerRehighlight(ctx, st, 0, progress);
        } else {
            // directly emit a state change
            emit stateChanged(ctx, st);
        }
    }
}

void Buffer::save(const QString &filepath)
{
    Progress progress(0, 0.5);
    setFilepath(filepath, _autodetectFiletype, progress);
    progress.cap = 1;
    emit workerSaveStateToFile(state(), _filepath, progress);
    setDirty(false);
}

void Buffer::load(const QString &filepath)
{
    Progress progress;
    setFilepath(filepath, false, progress);
    setLocked(true);
    // clear all the existing states
    _states.clear();
    StateChangeContext ctx(++_requestId);
    emit workerLoadStateFromFile(ctx, filepath, _autodetectFiletype, progress);
    setDirty(false);
}

const QSet<View *> &Buffer::views() const
{
    return _views;
}

void Buffer::attachView(View *view)
{
    _views.insert(view);
}

void Buffer::detachView(View *view)
{
    _views.remove(view);
}
