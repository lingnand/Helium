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

namespace srchilite {
    class LangMap;
}

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
    Q_SLOT void setName(const QString &name, bool rehighlightBuffer=true);
    const QString &filetype() const;
    Q_SLOT void setFiletype(const QString &filetype, bool rehighlightBuffer=true);
    BufferState &state();
    Q_SLOT void parseChange(View *source, const QString &content, ParserPosition start, int cursorPosition);
    Q_SLOT void parseReplacement(View *source, const Replacement &replace);
    Q_SLOT void parseReplacement(View *source, const QList<Replacement> &replaces);
    void killLine(int index);
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
    void workerMergeChange(BufferState &, View *, const BufferStateChange &);
    void workerSaveStateToFile(const BufferState &, const QString &);
    void workerLoadStateFromFile(const QString &);
private:
    bool _locked;
    QThread _workerThread;
    BufferWorker _worker;
    QString _name;
    BufferHistory _states;
    QDateTime _lastEdited;
    srchilite::LangMap *_langMap;

    // for highlighting
    HtmlBufferChangeParser _bufferChangeParser;

    BufferState &modifyState();
    void setLocked(bool);

    Q_SLOT void onWorkerChangeMerged(const BufferState &state, View *source, bool shouldUpdateSourceView);
    Q_SLOT void onWorkerStateLoadedFromFile(const BufferState &state, const QString &filename);
};

#endif /* BUFFER_H_ */
