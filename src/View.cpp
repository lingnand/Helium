/*
 * View.cpp
 *
 *  Created on: Nov 21, 2014
 *      Author: lingnan
 */

#include <src/View.h>
#include <src/Helper.h>
#include <src/Buffer.h>
#include <bb/cascades/Page>
#include <bb/cascades/TextArea>
#include <bb/cascades/TextField>
#include <bb/cascades/TitleBar>
#include <bb/cascades/TextFieldTitleBarKindProperties>
#include <bb/system/SystemToast>
#include <bb/system/SystemUiPosition>

using namespace bb::cascades;

#define EMPTY_BUFFER_TITLE "No Name"

View::View(Buffer* buffer):m_pBuffer(NULL)
{
    // the main text area within the page
    m_pTextArea = new TextArea;
    m_pTextArea->setTextFormat(TextFormat::Html);

    // title bar
    TitleBar *tb = new TitleBar(TitleBarKind::TextField);
    TextFieldTitleBarKindProperties *tbp = new TextFieldTitleBarKindProperties();
    m_pTitleTextField = tbp->textField();
    conn(m_pTitleTextField, SIGNAL(focusedChanged(bool)),
         this, SLOT(onTitleTextFieldFocusChanged(bool)));
    tb->setKindProperties(tbp);

    Page *p = new Page;
    p->setTitleBar(tb);
    p->setContent(m_pTextArea);

    setContent(p);

    // label initial load
    onLanguageChanged();

    // set the buffer
    setBuffer(buffer);
}

void View::setTitle(const QString& title)
{
    if (title.isEmpty()) {
        Tab::setTitle(tr(EMPTY_BUFFER_TITLE));
    } else {
        m_pTitleTextField->setText(title);
        Tab::setTitle(title);
    }
}

void View::setImageSource(const QUrl& image)
{
    if (m_pBuffer->imageSource().isEmpty()) {
        // use a default image
    } else {
        Tab::setImageSource(image);
    }
}

void View::setBuffer(Buffer* buffer)
{
    if (buffer != m_pBuffer) {
        if (m_pBuffer) {
            disconn(m_pBuffer, SIGNAL(nameChanged(QString)),
                    this, SLOT(setTitle(QString)));
            disconn(m_pBuffer, SIGNAL(imageSourceChanged(QUrl)),
                    this, SLOT(setImageSource(QUrl)));
            disconn(m_pTextArea, SIGNAL(textChanging(QString)),
                    this, SLOT(onTextAreaTextChanged(QString)));
            disconn(m_pBuffer, SIGNAL(parsedContentChanged(QString)),
                    m_pTextArea, SLOT(setText(QString)));
        }
        m_pBuffer = buffer;
        // set up the initial value for the title
        setTitle(m_pBuffer->name());
        setImageSource(m_pBuffer->imageSource());
        m_pTextArea->setText(m_pBuffer->parsedContent());
        conn(m_pBuffer, SIGNAL(nameChanged(QString)),
             this, SLOT(setTitle(QString)));
        conn(m_pBuffer, SIGNAL(imageSourceChanged(QUrl)),
             this, SLOT(setImageSource(QUrl)));
        conn(m_pTextArea, SIGNAL(textChanging(QString)),
             this, SLOT(onTextAreaTextChanged(QString)));
        conn(m_pBuffer, SIGNAL(parsedContentChanged(QString)),
             m_pTextArea, SLOT(setText(QString)));
    }
}

void View::onTitleTextFieldFocusChanged(bool focus)
{
    if (!focus) {
        // the user defocused the text field
        // set the buffer name
        m_pBuffer->setName(m_pTitleTextField->text());
    }
}

void View::onTextAreaTextChanged(const QString& text)
{
    fprintf(stdout, "text changed: %s\n", qPrintable(text));
    m_pBuffer->setContent(text);
}

void View::onTextAreaCursorPositionChanged(int position)
{
    fprintf(stdout, "cursor position changed: %d\n", position);
}

void View::onLanguageChanged()
{
    m_pTitleTextField->setHintText(tr("Enter the title"));
    m_pTextArea->setHintText(tr("Enter the content"));
    if (m_pBuffer) {
        // reset the default name
        setTitle(m_pBuffer->name());
    }
}
