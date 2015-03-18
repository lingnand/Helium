/*
 * Buffer.cpp
 *
 *  Created on: Nov 23, 2014
 *      Author: lingnan
 */

#include <QTextStream>
#include <src/Buffer.h>
#include <stdio.h>
#include <src/srchilite/instances.h>
#include <src/srchilite/langmap.h>
#include <src/HtmlPlainTextExtractor.h>
#include <src/SaveWork.h>

#define SECONDS_TO_REGISTER_HISTORY 1
#define DEFAULT_EDIT_TIME (QDateTime::fromTime_t(0))

Buffer::Buffer(int historyLimit) :
    _highlight("default.style"),
    _langMap(srchilite::Instances::getLangMap()),
    _emittingContentChange(false), _hasUndo(false), _hasRedo(false),
    _historyIndex(0),
    _historyLimit(historyLimit),
    _lastEdited(DEFAULT_EDIT_TIME),
    _worker(this)
{
    _history.append(BufferState("", 0, ""));
    conn(&_highlight, SIGNAL(filetypeChanged(QString)),
        this, SLOT(onHtmlHighlightFiletypeChanged(QString)));
    conn(&_worker, SIGNAL(inProgressChanged(float)),
        this, SIGNAL(inProgressChanged(float)));
}

Buffer::~Buffer()
{
    _worker.quit();
    _worker.wait();
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

const QString &Buffer::filetype() const { return _highlight.filetype(); }

void Buffer::setFiletype(const QString &filetype) 
{ 
    _highlight.setFiletype(filetype); 
    // TODO: rehighlight the current bufferstate
    QString cont(_content);
    QTextStream input(&cont);
    _content.clear();
    QTextStream output(&_content);
    updateContentForCurrentFiletype(input, output);
    output << flush;
    emitContentChanged(NULL, true, -1);
    emit filetypeChanged(filetype);
}

const QString &Buffer::content() const { return _content; }

bool Buffer::isEmittingContentChange() const { return _emittingContentChange; }

void Buffer::emitContentChanged(View *source, bool sourceChanged, int cursorPosition)
{
    _emittingContentChange = true;
    emit contentChanged(source, sourceChanged, _content, cursorPosition);
    _emittingContentChange = false;
}

void Buffer::onHtmlHighlightFiletypeChanged(const QString &filetype)
{
}

void Buffer::registerContentChange(int cursorPosition)
{
    // save history state
    QDateTime current = QDateTime::currentDateTime();
    if (_historyIndex < _history.count() - 1) {
        do {
            _history.removeLast();
        } while (_historyIndex < _history.count() - 1);
        ++_historyIndex;
    } else if (current >= _lastEdited.addSecs(SECONDS_TO_REGISTER_HISTORY)) {
        ++_historyIndex;
    }
    _lastEdited = current;
    // append/modify the history if necessary
    if (_historyIndex == _history.count()) {
        _history.append(BufferState(_content, cursorPosition, filetype()));
        if (_history.count() > _historyLimit) {
            _history.removeFirst();
            --_historyIndex;
        }
    } else {
        _history[_historyIndex] = BufferState(_content, cursorPosition, filetype());
    }
    printf("last edit time: %s, historyIndex: %d\n",
            qPrintable(_lastEdited.toString("hh:mm:ss:zzz")), _historyIndex);
    // there is always blank text state to undo to
    setHasUndo(true);
    setHasRedo(false);
}

// TODO: also tackle the case where the editor is moved, selection selected
// in other words, in such situations you also need to rehighlight any delayed content
void Buffer::parseIncrementalContentChange(View *source, const QString &content, int cursorPosition, bool enableDelay)
{
    bool sourceChanged = false;
    if (!filetype().isEmpty()) {
        // set up input
        QString cont(content);
        QTextStream input(&cont);
        if (cont.startsWith("<pre>"))
            input.seek(5);
//        if (content.endsWith("</pre>"))
//            content.chop(6);
//        QTextStream input(&content);

        _content.clear();
        QTextStream output(&_content);
        output << "<pre>";
        sourceChanged = _highlight.highlightHtml(input, output, cursorPosition, enableDelay);
        output << flush;
        if (!_content.endsWith("</pre>"))
            _content += "</pre>";
    } else {
        // we assume that when this function is called there is always some change
        _content = content;
    }
//        registerContentChange(cursorPosition);
    emitContentChanged(source, sourceChanged, -1);
}

void Buffer::parseReplacementContentChange(View *source, QList<QPair<TextSelection, QString> > &replaces)
{
    if (replaces.count() == 0)
        return;
    bool sourceChanged = false;
    QString cont(_content);
    QTextStream input(&cont);
    _content.clear();
    QTextStream output(&_content);
    if (filetype().isEmpty()) {
        sourceChanged = _extractor.replacePlainText(input, output, replaces);
    } else {
        sourceChanged = _highlight.replaceHtml(input, output, replaces);
    }
    output << flush;
//        registerContentChange(-1);
    emitContentChanged(source, sourceChanged, -1);
}

bool Buffer::hasUndo() const { return _hasUndo; }

void Buffer::setHasUndo(bool hasUndo)
{
    if (hasUndo != _hasUndo) {
        _hasUndo = hasUndo;
        emit hasUndosChanged(_hasUndo);
    }
}

bool Buffer::hasRedo() const { return _hasRedo; }

void Buffer::setHasRedo(bool hasRedo)
{
    if (hasRedo != _hasRedo) {
        _hasRedo = hasRedo;
        emit hasRedosChanged(_hasRedo);
    }
}

void Buffer::undo()
{
    goToHistory(-1);
}

void Buffer::redo()
{
    goToHistory(1);
}

void Buffer::goToHistory(int offset)
{
    int i = _historyIndex + offset;
    if (i >= 0 && i < _history.count()) {
        setHasUndo(i != 0);
        setHasRedo(i != _history.count() - 1);
        _historyIndex = i;
//        printf("new history index: %d\n", i);
        BufferState &s = _history[i];
        // flush the state hash
        _highlight.clearHighlightStateDataHash();
        // parse the content if necessary
        if (s.filetype != filetype()) {
//            printf("history state with different filetype! current: %s; history: %s\n",
//                    qPrintable(filetype()), qPrintable(s.filetype));
            s.filetype = filetype();
            QString cont(s.content);
            QTextStream input(&cont);
            QTextStream output(&s.content);
            updateContentForCurrentFiletype(input, output);
            output << flush;
//            printf("history slot after content reparse: content: %s; filetype: %s\n",
//                    qPrintable(_history[i].content), qPrintable(_history[i].filetype));
        }
        _content = s.content;
        emitContentChanged(NULL, true, s.cursorPosition);
//        printf("new content: %s, new cursor position: %d\n", qPrintable(content()), s.second);
        // invalidate the date time
        _lastEdited = DEFAULT_EDIT_TIME;
    }
}

/* file related operations */
void Buffer::save()
{
    if (!_worker.isRunning()) {
        BufferWorkPtr w = _worker.work();
        if (!w || w->type() != Save) {
            w = BufferWorkPtr(new SaveWork);
            _worker.setWork(w);
        }
        boost::dynamic_pointer_cast<SaveWork>(w)->setHtml(content());
        _worker.start();
    } else if (_worker.work()->type() != Save) {
        printf("save action activated while some other job %d is running!\n", _worker.work()->type());
    }
}

bool Buffer::hasPlainText()
{
    QTextStream input(&_content);
    return _extractor.hasPlainText(input);
}

QString Buffer::plainText()
{
    QTextStream input(&_content);
    QString cont;
    QTextStream output(&cont);
    _extractor.extractPlainText(input, output);
    output << flush;
    return cont;
}
