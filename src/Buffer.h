/*
 * Buffer.h
 *
 *  Created on: Nov 23, 2014
 *      Author: lingnan
 */

/*
 * The primary buffer model for text editing
 * It processes signals on a separate thread (its own thread)
 * TODO: tabs are always a headache.
 *  some problems to think about
 *  1. option to enable the user to view four spaces as a single tab when openning files
 *    (so when a user opens a source file that uses spaces instead of tabs,
 *    they would get the editing benefit and simplicity of tabs)
 *  2. option to enable the user to insert tab as spaces
 *  3. option to save tab as space
 *  - basically turning 1 on, 2 off, and 3 off would be the default
 *  - if the user likes spaces he can turn 3 on as well
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include <QUrl>
#include <QDateTime>
#include <QTextStream>
#include <QThread>
#include <boost/regex.hpp>
#include <StateChangeContext.h>
#include <BufferWorker.h>
#include <HtmlBufferChangeParser.h>
#include <BufferHistory.h>
#include <HighlightType.h>

class View;

class Buffer : public QObject
{
    Q_OBJECT
public:
    Buffer(int historyLimit=0, QObject *parent=NULL);
    virtual ~Buffer();
    // a locked buffer shouldn't get any of its functions triggered
    bool locked() const { return _locked; }
    // whether the buffer is dirty (have not been saved)
    bool dirty() const { return _dirty; }
    // returns the name of the current buffer, "" for no name
    const QString &name() const { return _name; }
    // the name the buffer will use to write to the filesystem when requested
    Q_SLOT void setName(const QString &name);
    bool autodetectFiletype() const { return _autodetectFiletype; }
    Q_SLOT void setAutodetectFiletype(bool);
    // the filepath of the buffer, empty if not yet bound
    const QString &filepath() const { return _filepath; }
    Filetype *filetype() { return state().filetype(); }
    HighlightType highlightType() { return state().highlightType(); }
    Q_SLOT void setFiletype(Filetype *);
    Q_SLOT void setHighlightStyleFile(const QString &);
    const BufferState &state() const { return _states.current(); }
    Q_SLOT void parseChange(View *source, const QString &content, ParserPosition start, int cursorPosition);
    Q_SLOT void parseReplacement(const Replacement &replace);
    Q_SLOT void parseReplacement(const QList<Replacement> &replaces);
    void killLine(View *source, int cursorPosition);
    bool hasUndo() { return _states.retractable(); }
    Q_SLOT void undo();
    bool hasRedo() { return _states.advanceable(); }
    Q_SLOT void redo();
    Q_SLOT void save(const QString &filepath);
    Q_SLOT void load(const QString &filepath);
    // views
    const QSet<View *> &views() const;
    void attachView(View *);
    void detachView(View *);
Q_SIGNALS:
    void lockedChanged(bool);
    void dirtyChanged(bool);
    void nameChanged(const QString &name);
    void filepathChanged(const QString &filepath);
    void autodetectFiletypeChanged(bool);
    void filetypeChanged(Filetype *change, Filetype *old);
    void stateChanged(const StateChangeContext &, const BufferState &);
    // this happens when a long-running operation is triggered
    // note that the progress will reset to 0 when the task is finished
    void progressChanged(float, bb::cascades::ProgressIndicatorState::Type, const QString &msg);
    void hasUndosChanged(bool hasUndos);
    void hasRedosChanged(bool hasRedos);
    // worker
    void workerSetHighlightType(StateChangeContext &, BufferState &, const HighlightType &, Progress &);
    void workerParseAndMergeChange(StateChangeContext &, BufferState &, const QString &content, ParserPosition, int cursorPosition, Progress &);
    void workerMergeChange(StateChangeContext &, BufferState &, const BufferStateChange &, Progress &);
    void workerReplace(StateChangeContext &, BufferState &, const QList<Replacement> &, Progress &);
    void workerRehighlight(StateChangeContext &, BufferState &, int index, Progress &);
    void workerSaveStateToFile(const BufferState &, const QString &filename, Progress &);
    void workerLoadStateFromFile(StateChangeContext &, const QString &filename, bool autodetectFiletype, Progress &);
    // task report
    void savedToFile(const QString &filename);
private:
    // store the list of attached views
    QSet<View *> _views;

    bool _dirty;
    bool _locked;
    QThread _workerThread;
    // requestId is a non-decreasing id that identifies
    // the current request to the worker
    uint _requestId;
    BufferWorker _worker;
    QString _name;
    bool _autodetectFiletype;
    QString _filepath;
    BufferHistory _states;
    QDateTime _lastEdited;

    BufferState &modifyState(bool forceCopy=false);
    void setLocked(bool);
    void setDirty(bool);

    QString _highlightStyleFile;

    void _setName(const QString &name, bool setHighlightType, Progress &);
    void setHighlightType(const HighlightType &, Progress &);
    void setFilepath(const QString &filepath, bool setHighlightType, Progress &);
    void traverse(bool (BufferHistory::*fn)());
    Q_SLOT void handleStateChangeResult(const StateChangeContext &, const BufferState &);
    Q_SLOT void refreshFiletype();
};

#endif /* BUFFER_H_ */
