/*
 * Buffer.cpp
 *
 *  Created on: Nov 23, 2014
 *      Author: lingnan
 */

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
    _history.append(BufferState("", 0));
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

void Buffer::setFiletype(const QString &filetype) { _highlight.setFiletype(filetype); }

bool Buffer::isEmittingContentChange() const { return _emittingContentChange; }

const QString &Buffer::content() const { return _content; }

void Buffer::setContent(const QString &content, int cursorPosition)
{
    _emittingContentChange = true;
    _content = content;
    emit contentChanged(_content, cursorPosition);
    _emittingContentChange = false;
}

void Buffer::onHtmlHighlightFiletypeChanged(const QString &filetype)
{
    // TODO: how do we clear the buffer states?
    // OR, make sure that the undo works with file type related changes
    // TODO: we can change the saved history state for the current one, so that
    // it matches the current file type (save re-parsing on the next whoop)
    if (filetype.isEmpty()) {
        setContent(plainText(), -1);
    } else {
        parseIncrementalContentChange(content(), 0, false);
    }
    emit filetypeChanged(filetype);
}

void Buffer::registerContentChange(const QString &cont, int cursorPosition)
{
    if (cont != content()) {
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
            _history.append(BufferState(cont, cursorPosition));
            if (_history.count() > _historyLimit) {
                _history.removeFirst();
                --_historyIndex;
            }
        } else {
            _history[_historyIndex] = BufferState(cont, cursorPosition);
        }
        printf("last edit time: %s, historyIndex: %d\n",
                qPrintable(_lastEdited.toString("hh:mm:ss:zzz")), _historyIndex);
        // there is always blank text state to undo to
        setHasUndo(true);
        setHasRedo(false);
        setContent(cont, -1);
    }
}

// TODO: also tackle the case where the editor is moved, selection selected
// in other words, in such situations you also need to rehighlight any delayed content
// TODO: the undo/redo might mess up our HighlightStateHash, so should we save a snapshot of the hash
// together with the undo/redo as well?
// an easier way is to flush the hash on redo/undo
// or we can re-index the lines so that the highlighter knows not to use the hash
void Buffer::parseIncrementalContentChange(QString cont, int cursorPosition, bool enableDelay)
{
    if (!filetype().isEmpty()) {
        cont = _highlight.highlightHtml(cont, cursorPosition, enableDelay);
    }
    registerContentChange(cont, cursorPosition);
}

void Buffer::parseReplacementContentChange(QList<QPair<TextSelection, QString> > replaces)
{
    QString cont;
    if (filetype().isEmpty()) {
        cont = _extractor.replacePlainText(content(), replaces);
    } else {
        cont = _highlight.replaceHtml(content(), replaces);
    }
    registerContentChange(cont, -1);
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
    printf("## go to history with offset %d ##\n", offset);
    int i = _historyIndex + offset;
    if (i >= 0 && i < _history.count()) {
        setHasUndo(i != 0);
        setHasRedo(i != _history.count() - 1);
        _historyIndex = i;
        printf("new history index: %d\n", i);
        BufferState s = _history[i];
        setContent(s.first, s.second);
        printf("new content: %s, new cursor position: %d\n", qPrintable(content()), s.second);
        // invalidate the date time
        _lastEdited = DEFAULT_EDIT_TIME;
    }
    printf("## go to history with offset %d finished ##\n", offset);
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
    return _extractor.hasPlainText(content());
}

QString Buffer::plainText()
{
    return _extractor.extractPlainText(content());
}
