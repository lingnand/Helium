/*
 * Buffer.h
 *
 *  Created on: Nov 23, 2014
 *      Author: lingnan
 */

/*
 * The primary buffer model for text editing
 * It processes signals on a separate thread (its own thread)
 * TODO: think about the race conditions that can be caused
 *  e.g. somebody sent a "save" signal here and the background process begins saving
 *  whereas somebody immediately changed the buffer content
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
#include <QThread>
#include <boost/shared_ptr.hpp>
#include <src/HtmlHighlight.h>
#include <src/HtmlPlainTextExtractor.h>

namespace srchilite {
    class LangMap;
}

class Buffer : public QObject
{
    Q_OBJECT
public:
    Buffer();
    virtual ~Buffer();
    // returns the name of the current buffer, "" for no name
    const QString &name() const { return _name; }
    const QString &filetype() const { return _highlight.filetype(); }
    const QString &content() const { return _content; }
    // the name the buffer will use to write to the filesystem when requested
    Q_SLOT void setName(const QString &name);
    Q_SLOT void setFiletype(const QString &filetype) { _highlight.setFiletype(filetype); }
    Q_SLOT void setContent(const QString &content, int cursorPosition);
    Q_SLOT void save();
Q_SIGNALS:
    void nameChanged(const QString &name);
    void filetypeChanged(const QString &filetype);
    void contentChanged(const QString &parsedContent);
    // this happens when a long-running operation is triggered
    // note that the progress will reset to 0 when the task is finished
    void inProgressChanged(float);
private:
    bool _parsingContent;
    QString _name;
    QString _content;
    // the thread the buffer lives in
    QThread _thread;
    HtmlHighlight _highlight;
    HtmlPlainTextExtractor _extractor;
    srchilite::LangMap *_langMap;
    void parseContent(const QString content, int cursorPosition, bool noHighlight, bool enableDelay);
    Q_SLOT void onHtmlHighlightFiletypeChanged(const QString &filetype);
};

#endif /* BUFFER_H_ */
