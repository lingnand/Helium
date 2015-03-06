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
#include <src/HtmlHighlight.h>
#include <src/BufferWorker.h>
#include <src/HtmlPlainTextExtractor.h>

namespace srchilite {
    class LangMap;
}

class View;

class Buffer : public QObject
{
    Q_OBJECT
public:
    Buffer(int historyLimit);
    virtual ~Buffer();
    // returns the name of the current buffer, "" for no name
    const QString &name() const;
    // the name the buffer will use to write to the filesystem when requested
    Q_SLOT void setName(const QString &name);
    const QString &filetype() const;
    Q_SLOT void setFiletype(const QString &filetype);
    bool isEmittingContentChange() const;
    const QString &content() const;
    Q_SLOT void parseIncrementalContentChange(View *source, const QString &content, int cursorPosition, bool enableDelay);
    Q_SLOT void parseReplacementContentChange(View *source, QList<QPair<TextSelection, QString> > &replaces);
    bool hasUndo() const;
    Q_SLOT void undo();
    bool hasRedo() const;
    Q_SLOT void redo();
    bool hasPlainText();
    QString plainText();
    Q_SLOT void save();
Q_SIGNALS:
    void nameChanged(const QString &name);
    void filetypeChanged(const QString &filetype);
    // on content changed, negative cursorPosition means the changed content
    // doesn't change cursor position
    // otherwise the view should set its cursor position to match the value emitted here
    void contentChanged(View *source, bool sourceChanged, const QString &content, int cursorPosition);
    // this happens when a long-running operation is triggered
    // note that the progress will reset to 0 when the task is finished
    void inProgressChanged(float progress);
    void hasUndosChanged(bool hasUndos);
    void hasRedosChanged(bool hasRedos);
private:
    struct BufferState {
        QString content;
        int cursorPosition;
        QString filetype;
        BufferState(QString _content, int _cursorPosition, QString _filetype):
            content(_content), cursorPosition(_cursorPosition), filetype(_filetype) {}
    };
    bool _emittingContentChange;
    bool _hasUndo;
    bool _hasRedo;
    QString _name;
    QString _content;
    int _historyIndex;
    int _historyLimit;
    QList<BufferState> _history;
    QDateTime _lastEdited;
    BufferWorker _worker;
    HtmlHighlight _highlight;
    HtmlPlainTextExtractor _extractor;
    srchilite::LangMap *_langMap;
    void goToHistory(int offset);
    void setHasUndo(bool hasUndo);
    void setHasRedo(bool hasRedo);
    Q_SLOT void updateContentForCurrentFiletype(QTextStream &input, QTextStream &output);
    Q_SLOT void onHtmlHighlightFiletypeChanged(const QString &filetype);
    Q_SLOT void emitContentChanged(View *source, bool sourceChanged, int cursorPosition);
    Q_SLOT void registerContentChange(int cursorPosition);
};

#endif /* BUFFER_H_ */
