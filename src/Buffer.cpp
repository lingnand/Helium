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

Buffer::Buffer() :
    _highlight("default.style"),
    _langMap(srchilite::Instances::getLangMap()),
    _parsingContent(false)
{
    conn(&_highlight, SIGNAL(filetypeChanged(QString)),
        this, SLOT(onHtmlHighlightFiletypeChanged(QString)));
    moveToThread(&_thread);
    _thread.start();
}

Buffer::~Buffer()
{
    _thread.quit();
    _thread.wait();
}

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

void Buffer::onHtmlHighlightFiletypeChanged(const QString &filetype)
{
    parseContent(_content, 0, true, false);
    emit filetypeChanged(filetype);
}

void Buffer::setContent(const QString &content, int cursorPosition)
{
    parseContent(content, cursorPosition, false, true);
}

void Buffer::parseContent(const QString content, int cursorPosition, bool forceHighlight, bool enableDelay)
{
    if (_parsingContent)
        return;
    _parsingContent = true;
    if (forceHighlight || !filetype().isEmpty()) {
        _content = _highlight.highlightHtml(content, cursorPosition, enableDelay);
        emit contentChanged(_content);
    } else if (content != _content) {
        // if highlight is not necessary
        _content = content;
        emit contentChanged(_content);
    }
    _parsingContent = false;
}

/* file related operations */
void Buffer::save()
{
    // get the original text of the content
    QString toSave =_extractor.extractPlainText(_content);
    printf("toSave: %s\n", qPrintable(toSave));
    int max = 1000;
    int msec = 3000;
    int i = 0;
    while (i < max) {
        i++;
        emit inProgressChanged(i/(float)max);
        usleep(msec);
    }
    emit inProgressChanged(0);
}
