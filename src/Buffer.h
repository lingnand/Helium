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

class View;

class Buffer : public QObject
{
    Q_OBJECT
public:
    Buffer(int historyLimit = 0);
    virtual ~Buffer();
    // a locked buffer shouldn't get any of its functions triggered
    bool locked() const;
    // whether the buffer is dirty (have not been saved)
    bool dirty() const;
    // returns the name of the current buffer, "" for no name
    const QString &name() const;
    // the name the buffer will use to write to the filesystem when requested
    Q_SLOT void setName(const QString &name, bool setFiletype=true);
    const QString &filepath() const; // the filepath of the buffer, empty if not yet bound
    const QString &filetype() const;
    Q_SLOT void setFiletype(const QString &filetype);
    const BufferState &state() const;
    Q_SLOT void parseChange(View *source, const QString &content, ParserPosition start, int cursorPosition);
    Q_SLOT void parseReplacement(const Replacement &replace);
    Q_SLOT void parseReplacement(const QList<Replacement> &replaces);
    void killLine(View *source, int cursorPosition);
    bool hasUndo();
    Q_SLOT void undo();
    bool hasRedo();
    Q_SLOT void redo();
    Q_SLOT void save(const QString &filepath);
    Q_SLOT void load(const QString &filepath);
Q_SIGNALS:
    void lockedChanged(bool);
    void dirtyChanged(bool);
    void nameChanged(const QString &name);
    void filetypeChanged(const QString &filetype);
    void stateChanged(const StateChangeContext &, const BufferState &);
    // this happens when a long-running operation is triggered
    // note that the progress will reset to 0 when the task is finished
    void progressChanged(float, bb::cascades::ProgressIndicatorState::Type, const QString &msg);
    void hasUndosChanged(bool hasUndos);
    void hasRedosChanged(bool hasRedos);
    // worker
    void workerInitialize();
    void workerSetFiletype(StateChangeContext &, BufferState &, const QString &filetype);
    void workerParseAndMergeChange(StateChangeContext &, BufferState &, const QString &content, ParserPosition, int cursorPosition, bool trackProgress=true);
    void workerMergeChange(StateChangeContext &, BufferState &, const BufferStateChange &, bool trackProgress=true);
    void workerReplace(StateChangeContext &, BufferState &, const QList<Replacement> &);
    void workerRehighlight(StateChangeContext &, BufferState &, int index=0);
    void workerSaveStateToFile(const BufferState &, const QString &filename);
    void workerLoadStateFromFile(StateChangeContext &, const QString &filename);
private:
    bool _dirty;
    bool _locked;
    QThread _workerThread;
    // requestId is a non-decreasing id that identifies
    // the current request to the worker
    unsigned int _requestId;
    BufferWorker _worker;
    QString _name;
    QString _filepath;
    BufferHistory _states;
    QDateTime _lastEdited;

    BufferState &modifyState();
    void setLocked(bool);
    void setDirty(bool);

    void setFilepath(const QString &filepath, bool setFiletype=true);
    void traverse(bool (BufferHistory::*fn)());
    Q_SLOT void handleStateChangeResult(const StateChangeContext &, const BufferState &);
};

#endif /* BUFFER_H_ */
