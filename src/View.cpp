/*
 * View.cpp
 *
 *  Created on: Nov 21, 2014
 *      Author: lingnan
 */

#include <src/View.h>
#include <src/Buffer.h>
#include <bb/cascades/Page>
#include <bb/cascades/TextArea>
#include <bb/cascades/KeyEvent>
#include <bb/cascades/KeyListener>
#include <bb/cascades/TextField>
#include <bb/cascades/TextInputProperties>
#include <bb/cascades/TitleBar>
#include <bb/cascades/TextFieldTitleBarKindProperties>
#include <bb/system/SystemToast>
#include <bb/system/SystemUiPosition>

using namespace bb::cascades;

#define EMPTY_BUFFER_TITLE "No Name"
// keys
#define KEYFLAG_NONE 0
#define KEYFLAG_RETURN 1

View::View(Buffer* buffer):_buffer(NULL), _modPressed(KEYFLAG_NONE), _modUsed(KEYFLAG_NONE)
{
    // the main text area within the page
    _textArea = new TextArea;
    _textArea->setTextFormat(TextFormat::Html);
//    _textArea->setInputMode(TextAreaInputMode::Custom);
//    _textArea->setInputMode(TextAreaInputMode::Text);
//    _textArea->input()->setFlags(TextInputFlag::PredictionOff |
//                                 TextInputFlag::AutoCorrectionOff |
//                                 TextInputFlag::AutoCapitalizationOff |
//                                 TextInputFlag::AutoPeriodOff |
//                                 TextInputFlag::SpellCheckOff |
//                                 TextInputFlag::WordSubstitutionOff);
//    KeyListener *keyListener = new KeyListener;
//    conn(keyListener, SIGNAL(keyEvent(bb::cascades::KeyEvent*)),
//         this, SLOT(onTextAreaKeyEvent(bb::cascades::KeyEvent*)));
//    _textArea->addKeyListener(keyListener);

    // title bar
    TitleBar *tb = new TitleBar(TitleBarKind::TextField);
    TextFieldTitleBarKindProperties *tbp = new TextFieldTitleBarKindProperties();
    _titleTextField = tbp->textField();
    conn(_titleTextField, SIGNAL(focusedChanged(bool)),
         this, SLOT(onTitleTextFieldFocusChanged(bool)));
    TextInputProperties *input = _titleTextField->input();
    input->setSubmitKey(SubmitKey::Done);
    conn(input, SIGNAL(submitted(bb::cascades::AbstractTextControl*)),
         this, SLOT(onTitleTextFieldSubmitted(bb::cascades::AbstractTextControl*)));
    tb->setKindProperties(tbp);

    Page *p = new Page;
    p->setTitleBar(tb);
    p->setContent(_textArea);

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
        _titleTextField->setText(title);
        Tab::setTitle(title);
    }
}

void View::setBuffer(Buffer* buffer)
{
    if (buffer != _buffer) {
        if (_buffer) {
            disconn(_buffer, SIGNAL(nameChanged(QString)),
                    this, SLOT(setTitle(QString)));
            disconn(_textArea, SIGNAL(textChanging(QString)),
                    this, SLOT(onTextAreaTextChanged(QString)));
            disconn(_buffer, SIGNAL(filetypeChanged(QString)),
                    this, SLOT(onBufferFiletypeChanged(QString)));
            disconn(_buffer, SIGNAL(contentChanged(QString)),
                    this, SLOT(onBufferContentChanged(QString)));
        }
        _buffer = buffer;
        // set up the initial value for the title
        setTitle(_buffer->name());
        onBufferFiletypeChanged(_buffer->filetype());
        onBufferContentChanged(_buffer->content());
        conn(_buffer, SIGNAL(nameChanged(QString)),
             this, SLOT(setTitle(QString)));
        conn(_textArea, SIGNAL(textChanging(QString)),
             this, SLOT(onTextAreaTextChanged(QString)));
        conn(_buffer, SIGNAL(filetypeChanged(QString)),
             this, SLOT(onBufferFiletypeChanged(QString)));
        conn(_buffer, SIGNAL(contentChanged(QString)),
             this, SLOT(onBufferContentChanged(QString)));
    }
}

void View::onBufferFiletypeChanged(const QString& filetype) {
    if (filetype.isEmpty()) {
        // use a default image
        // TODO: fill in the images
//        setImageSource(QUrl("asset:///images/txt.png"));
    } else {
//        setImageSource(QUrl("asset:///images/"+filetype+".png"));
    }
}

void View::onBufferContentChanged(const QString& content) {
    if (_textArea->text() != content) {
        printf("## text area out of sync with buffer content\n### text area: %s\n### buffer: %s\n", qPrintable(_textArea->text()), qPrintable(content));
        int oldPos = _textArea->editor()->cursorPosition();
        _textArea->setText(content);
        _textArea->editor()->setCursorPosition(oldPos);
    }
}

void View::onTitleTextFieldSubmitted(AbstractTextControl *control)
{
    _textArea->requestFocus();
}

void View::onTitleTextFieldFocusChanged(bool focus)
{
    if (!focus) {
        // the user defocused the text field
        // set the buffer name
        _buffer->setName(_titleTextField->text());
    }
}

void View::onTextAreaTextChanged(const QString& text)
{
//    fprintf(stdout, "text changed: %s\n", qPrintable(text));
//    if (_textArea->editor()->selectedText().isEmpty())
    // only when the the cursor is currently before
    _buffer->setContent(text, _textArea->editor()->cursorPosition(), true);
}

void View::onTextAreaKeyEvent(bb::cascades::KeyEvent *event)
{
    if (event->isPressed()) {
        if ((_modPressed & KEYFLAG_RETURN) == KEYFLAG_RETURN) {
            switch (event->keycap()) {
                case KEYCODE_S: {
                    bb::system::SystemToast *toast = new bb::system::SystemToast(this);
                    toast->setBody("Save file");
                    toast->setPosition(bb::system::SystemUiPosition::MiddleCenter);
                    toast->show();
                    _modUsed |= KEYFLAG_RETURN;
                    break;
                }
            }
        } else {
            switch (event->keycap()) {
                case KEYCODE_RETURN:
                    printf("return pressed!\n");
                    _modPressed |= KEYFLAG_RETURN;
                    break;
                case KEYCODE_BACKSPACE:
                    printf("backspace pressed!\n");
//                    _textArea->editor()->insertPlainText(event->unicode());
                    break;
                case KEYCODE_DELETE:
                    printf("delete pressed!\n");
                    break;
                default:
                    // all other keys immediately trigger a write event
                    _textArea->editor()->insertPlainText(event->unicode());
            }
            printf("unicode is %s\n", qPrintable(event->unicode()));
        }
    } else {
        switch (event->keycap()) {
            case KEYCODE_RETURN:
                if ((_modPressed & KEYFLAG_RETURN) == KEYFLAG_RETURN) {
                    _modPressed ^= KEYFLAG_RETURN;
                    if ((_modUsed & KEYFLAG_RETURN) == KEYFLAG_RETURN) {
                        // Return has been used as a mod key
                        _modUsed ^= KEYFLAG_RETURN;
                    } else {
                        _textArea->editor()->insertPlainText(event->unicode());
                    }
                }
                break;
        }
    }
}

void View::onLanguageChanged()
{
    _titleTextField->setHintText(tr("Enter the title"));
    _textArea->setHintText(tr("Enter the content"));
    if (_buffer) {
        // reset the default name
        setTitle(_buffer->name());
    }
}
