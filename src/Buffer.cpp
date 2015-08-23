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
#include <FiletypeMap.h>
#include <Filetype.h>
#include <Utility.h>
#include <AppearanceSettings.h>

#define SECONDS_TO_REGISTER_HISTORY 1
#define DEFAULT_EDIT_TIME (QDateTime::fromTime_t(0))
#define DEFAULT_CHECKPOINT_TIME (QDateTime::fromTime_t(0))

// Buffer
Buffer::Buffer(int historyLimit, QObject *parent):
    QObject(parent),
    _requestId(0),
    _states(historyLimit),
    _lastEdited(DEFAULT_EDIT_TIME),
    _locked(false), _dirty(false),
    _autodetectFiletype(true),
    _lastCheckPoint(DEFAULT_CHECKPOINT_TIME)
{
    AppearanceSettings *appearanceSettings = Helium::instance()->appearance();
    _highlightStyleFile = appearanceSettings->highlightStyleFile();
    conn(appearanceSettings, SIGNAL(highlightStyleFileChanged(const QString&)),
        this, SLOT(setHighlightStyleFile(const QString&)));

    _worker.moveToThread(&_workerThread);
    conn(this, SIGNAL(workerSetHighlightType(StateChangeContext&, BufferState&, const HighlightType&, Progress)),
         &_worker, SLOT(setHighlightType(StateChangeContext&, BufferState&, const HighlightType&, Progress)));
    conn(this, SIGNAL(workerParseAndMergeChange(StateChangeContext&, BufferState&, const QString&, ParserPosition, int, Progress)),
         &_worker, SLOT(parseAndMergeChange(StateChangeContext&, BufferState&, const QString&, ParserPosition, int, Progress)));
    conn(this, SIGNAL(workerMergeChange(StateChangeContext&, BufferState&, const BufferStateChange&)),
         &_worker, SLOT(mergeChange(StateChangeContext&, BufferState&, const BufferStateChange&)));
    conn(this, SIGNAL(workerReplace(StateChangeContext&, BufferState&, const QList<Replacement>&)),
         &_worker, SLOT(replace(StateChangeContext&, BufferState&, const QList<Replacement>&)));
    conn(this, SIGNAL(workerRehighlight(StateChangeContext&, BufferState&, int)),
         &_worker, SLOT(rehighlight(StateChangeContext&, BufferState&, int)));
    conn(this, SIGNAL(workerSaveStateToFile(const BufferState&, const QString&, Progress)),
         &_worker, SLOT(saveStateToFile(const BufferState&, const QString&, Progress)));
    conn(this, SIGNAL(workerLoadStateFromFile(StateChangeContext&, const QString&, bool, Progress)),
         &_worker, SLOT(loadStateFromFile(StateChangeContext&, const QString&, bool, Progress)));

    conn(&_worker, SIGNAL(progressChanged(float, bb::cascades::ProgressIndicatorState::Type, const QString&)),
            this, SIGNAL(progressChanged(float, bb::cascades::ProgressIndicatorState::Type, const QString&)));
    conn(&_worker, SIGNAL(highlightTypeChanged(const StateChangeContext&, const BufferState&)),
            this, SLOT(handleStateChangeResult(const StateChangeContext&, const BufferState&)));
    conn(&_worker, SIGNAL(changeMerged(const StateChangeContext&, const BufferState&)),
            this, SLOT(handleStateChangeResult(const StateChangeContext&, const BufferState&)));
    conn(&_worker, SIGNAL(occurrenceReplaced(const StateChangeContext&, const BufferState&)),
            this, SLOT(handleStateChangeResult(const StateChangeContext&, const BufferState&)));
    conn(&_worker, SIGNAL(stateRehighlighted(const StateChangeContext&, const BufferState&)),
            this, SLOT(handleStateChangeResult(const StateChangeContext&, const BufferState&)));
    conn(&_worker, SIGNAL(stateLoadedFromFile(const StateChangeContext&, const BufferState&, const QString&)),
            this, SLOT(onStateLoadedFromFile(const StateChangeContext&, const BufferState&, const QString&)));
    conn(&_worker, SIGNAL(stateSavedToFile(const QString&)),
            this, SLOT(onStateSavedToFile(const QString&)));
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
    setName(name, _autodetectFiletype);
}

void Buffer::setName(const QString &name, bool sethl, Progress progress)
{
    if (name != _name) {
        _name = name;
        if (sethl)
            setHighlightType(HighlightType(_highlightStyleFile,
                    Helium::instance()->filetypeMap()->filetypeForName(name)), progress);
        setDirty(true);
        emit nameChanged(name);
    }
}

void Buffer::setFilepath(const QString &filepath, bool setHighlightType, Progress progress)
{
    if (filepath != _filepath) {
        _filepath = filepath;
        QFileInfo f(_filepath);
        if (f.exists())
            _lastCheckPoint = f.lastModified();
        else
            _lastCheckPoint = DEFAULT_CHECKPOINT_TIME;
        setName(QFileInfo(_filepath).fileName(), setHighlightType, progress);
        emit filepathChanged(_filepath);
    }
}

void Buffer::setFiletype(Filetype *filetype)
{
    setHighlightType(HighlightType(_highlightStyleFile, filetype));
}

void Buffer::refreshFiletype()
{
    setFiletype(filetype());
}

void Buffer::setHighlightStyleFile(const QString &style)
{
    if (style != _highlightStyleFile) {
        _highlightStyleFile = style;
        setHighlightType(HighlightType(_highlightStyleFile, filetype()));
    }
}

void Buffer::setHighlightType(const HighlightType &type, Progress progress)
{
    BufferState &st = _states.current();
    if (type != st.highlightType()) {
        StateChangeContext ctx(++_requestId);
        if (!st.isEmpty()) {
            // if state is empty we don't need to lock the textArea -- really
            setLocked(true);
        }
        emit workerSetHighlightType(ctx, st, type, progress);
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
    Filetype *oldFt = filetype(), *newFt = newSt.filetype();
    _states.current() = newSt;
    if (newFt != oldFt) {
        if (oldFt) {
            oldFt->disconnect(this);
        }
        if (newFt) {
            conn(newFt, SIGNAL(highlightEnabledChanged(bool)),
                this, SLOT(refreshFiletype()));
        }
        emit filetypeChanged(newFt, oldFt);
    }
    setLocked(false);
    emit stateChanged(ctx, newSt);
}

BufferState &Buffer::modifyState(bool forceCopy)
{
    // save history state
    QDateTime current = QDateTime::currentDateTime();
    if (forceCopy || current >= _lastEdited.addSecs(SECONDS_TO_REGISTER_HISTORY)) {
        _states.copyCurrent();
    }
    _lastEdited = current;
    setDirty(true);
    return _states.current();
}

void Buffer::parseChange(View *source, const QString &content, ParserPosition start, int cursorPosition)
{
    StateChangeContext ctx(++_requestId, source);
    BufferState &state = modifyState();
    if (state.highlightType().shouldHighlight()) {
        BufferStateChange change = _worker.parseBufferChange(state, content, start, cursorPosition);
        qDebug() << "change:" << change;
        qDebug() << "changeSize:" << change.size();
        if (change.size() > 100) { // put this into background
            setLocked(true);
            emit workerMergeChange(ctx, state, change);
        } else {
            SignalBlocker blocker(&_worker);
            _worker.mergeChange(ctx, state, change);
            emit stateChanged(ctx, state);
        }
    } else {
        // for empty filetype we can process the change in the background
        // without locking because each change is considered complete
        // furthermore, because of completeness we can throw away the
        // current state completely
        BufferState empty;
        emit workerParseAndMergeChange(ctx, empty, content, start, cursorPosition,
                Progress(0, 0));
    }
}

void Buffer::parseReplacement(const Replacement &replace)
{
    parseReplacement(QList<Replacement>() << replace);
}

void Buffer::parseReplacement(const QList<Replacement> &replaces)
{
    if (!replaces.empty() && !state().empty()) {
        StateChangeContext ctx(++_requestId);
        BufferState &state = modifyState(true);
        if (replaces.size() > 20) { // put this into background
            setLocked(true);
            emit workerReplace(ctx, state, replaces);
        } else {
            SignalBlocker blocker(&_worker);
            _worker.replace(ctx, state, replaces);
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
        BufferState &state = modifyState(true);
        state.setCursorPosition(cursorPosition - pos.linePosition);
        state[pos.lineIndex].line.clear();
        // blocking rehighlight (we assume the change is small)
        SignalBlocker blocker(&_worker);
        _worker.rehighlight(ctx, state, pos.lineIndex);
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

void Buffer::reload()
{
    if (_filepath.isEmpty()) {
        Utility::toast(tr("Buffer is not saved"));
        return;
    }
    if (!QFileInfo(_filepath).exists()) {
        Utility::toast(tr("File has been removed externally"));
        setDirty(true);
        return;
    }
    if (_dirty) {
        Utility::dialog(tr("Yes"), tr("No"), tr("Unsaved change detected"),
                tr("Do you want to abandon the changes and reload?"),
                this, SLOT(onUnsavedChangeDialogFinishedWhenReloading(bb::system::SystemUiResult::Type)));
    } else {
        load(_filepath, false);
    }
}

void Buffer::onUnsavedChangeDialogFinishedWhenReloading(bb::system::SystemUiResult::Type type)
{
    if (type == bb::system::SystemUiResult::ConfirmButtonSelection)
        load(_filepath);
}

void Buffer::traverse(bool (BufferHistory::*fn)())
{
    HighlightType highlightType = state().highlightType();
    if ((_states.*fn)()) {
        StateChangeContext ctx(++_requestId, NULL, true, true);
        setDirty(true);
        BufferState &st = _states.current();
        _lastEdited = DEFAULT_EDIT_TIME;
        if (st.highlightType() != highlightType) {
            // rehighlight in the background
            setLocked(true);
            emit workerRehighlight(ctx, st, 0);
        } else {
            // directly emit a state change
            emit stateChanged(ctx, st);
        }
    }
}

void Buffer::save(const QString &filepath)
{
    setFilepath(filepath, _autodetectFiletype, Progress(0, 0.5));
    emit workerSaveStateToFile(state(), _filepath, Progress(0.5));
}

void Buffer::load(const QString &filepath, bool resetCursor)
{
    setFilepath(filepath, false, Progress(0, 0.2));
    setLocked(true);
    // clear all the existing states
    _states.clear();
    StateChangeContext ctx(++_requestId, NULL, true, resetCursor);
    emit workerLoadStateFromFile(ctx, filepath, _autodetectFiletype, Progress(0.2));
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

void Buffer::onStateLoadedFromFile(const StateChangeContext &ctx, const BufferState &state, const QString &)
{
    setDirty(false);
    handleStateChangeResult(ctx, state);
}

void Buffer::onStateSavedToFile(const QString &)
{
    _lastCheckPoint = QDateTime::currentDateTime();
    setDirty(false);
    Utility::toast(tr("Saved"));
}

void Buffer::checkExternalChange()
{
    if (!_filepath.isEmpty()) {
        QFileInfo f(_filepath);
        if (!f.exists()) {
            Utility::toast(tr("File has been removed externally"));
            setDirty(true);
            return;
        }
        QDateTime lastM = f.lastModified();
        if (lastM > _lastCheckPoint.addSecs(2)) {
            _lastCheckPoint = lastM;
            Utility::dialog(tr("Reload"), tr("Ignore"), tr("External change detected"),
                    tr("File \"%1\" changed on disk. Reload?").arg(Utility::shortenPath(_filepath)),
                    this, SLOT(onReloadDialogFinishedForExternalChange(bb::system::SystemUiResult::Type)));
        }
    }
}

void Buffer::onReloadDialogFinishedForExternalChange(bb::system::SystemUiResult::Type type)
{
    if (type == bb::system::SystemUiResult::ConfirmButtonSelection)
        reload();
    else
        setDirty(true);
}
