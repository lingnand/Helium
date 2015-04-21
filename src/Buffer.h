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

#include <QObject>
#include <QUrl>
#include <QDateTime>
#include <QTextStream>
#include <QThread>
#include <boost/regex.hpp>
#include <src/BufferWorker.h>
#include <src/HtmlBufferChangeParser.h>
#include <src/BufferHistory.h>

class View;

class Buffer : public QObject
{
    Q_OBJECT
public:
    Buffer(int historyLimit = 0);
    virtual ~Buffer();
    // a locked buffer shouldn't get any of its functions triggered
    bool locked() const;
    // returns the name of the current buffer, "" for no name
    const QString &name() const;
    // the name the buffer will use to write to the filesystem when requested
    Q_SLOT void setName(const QString &name);
    Q_SLOT void setFiletype(const QString &filetype);
    BufferState &state();
    Q_SLOT void parseChange(View *source, const QString &content, ParserPosition start, int cursorPosition);
    Q_SLOT void parseReplacement(const Replacement &replace);
    Q_SLOT void parseReplacement(const QList<Replacement> &replaces);
    void killLine(View *source, int cursorPosition);
    bool hasUndo();
    Q_SLOT void undo();
    bool hasRedo();
    Q_SLOT void redo();
    Q_SLOT void save();
Q_SIGNALS:
    void lockedChanged(bool);
    void nameChanged(const QString &name);
    void filetypeChanged(const QString &filetype);
    void stateChanged(BufferState &state, View *source, bool sourceChanged, bool shouldMatchCursorPosition);
    // this happens when a long-running operation is triggered
    // note that the progress will reset to 0 when the task is finished
    void inProgressChanged(float progress);
    void hasUndosChanged(bool hasUndos);
    void hasRedosChanged(bool hasRedos);
    // worker
    void workerInitialize();
    void workerSetFiletype(unsigned int, BufferState &, const QString &);
    void workerParseAndMergeChange(unsigned int, BufferState &, View *, const QString &, ParserPosition, int);
    void workerMergeChange(unsigned int, BufferState &, View *, const BufferStateChange &);
    void workerReplace(unsigned int, BufferState &, const QList<Replacement> &);
    void workerRehighlight(unsigned int, BufferState &, View *source=NULL, int index=0, bool shouldMatchCursorPosition=false);
    void workerSaveStateToFile(const BufferState &, const QString &);
    void workerLoadStateFromFile(const QString &);
private:
    bool _locked;
    QThread _workerThread;
    // requestId is a non-decreasing id that identifies
    // the current request to the worker
    unsigned int _requestId;
    BufferWorker _worker;
    QString _name;
    BufferHistory _states;
    QDateTime _lastEdited;

    BufferState &modifyState();
    void setLocked(bool);

    Q_SLOT void onWorkerNoUpdate(unsigned int requestId);
    Q_SLOT void onWorkerStateUpdated(unsigned int requestId, const BufferState &state, View *source, bool shouldUpdateSourceView, bool shouldMatchCursorPosition);
    Q_SLOT void onWorkerStateLoadedFromFile(const BufferState &state, const QString &filename);
};

#endif /* BUFFER_H_ */
