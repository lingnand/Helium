/*
 * View.cpp
 *
 *  Created on: Nov 21, 2014
 *      Author: lingnan
 */

#include <src/View.h>
#include <src/Buffer.h>
#include <bb/cascades/Page>
#include <bb/cascades/Container>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/ProgressIndicator>
#include <bb/cascades/TextArea>
#include <bb/cascades/KeyEvent>
#include <bb/cascades/KeyListener>
#include <bb/cascades/TextField>
#include <bb/cascades/TextInputProperties>
#include <bb/cascades/TitleBar>
#include <bb/cascades/TextFieldTitleBarKindProperties>
#include <bb/cascades/ActionItem>
#include <bb/cascades/Shortcut>
#include <bb/system/SystemToast>
#include <bb/system/SystemUiPosition>

using namespace bb::cascades;

#define EMPTY_BUFFER_TITLE "No Name"
// keys
#define MOD_THRESHOLD 100
#define KEYFLAG_NONE 0
#define KEYFLAG_RETURN 1

View::View(Buffer* buffer):
        _titleTextField(NULL), _textArea(NULL), _progressIndicator(NULL),
        _buffer(NULL),
        _modPressed(KEYFLAG_NONE)
{
    // the main text area within the page
//    _textArea->input()->setSubmitKey(SubmitKey::Default);
//    conn(_textArea->input(), SIGNAL(submitted(bb::cascades::AbstractTextControl*)),
//         this, SLOT(onTextAreaReturnPressed()));

//    _textArea->setInputMode(TextAreaInputMode::Custom);
//    _textArea->setInputMode(TextAreaInputMode::Text);
//    _textArea->input()->setFlags(TextInputFlag::PredictionOff |
//                                 TextInputFlag::AutoCorrectionOff |
//                                 TextInputFlag::AutoCapitalizationOff |
//                                 TextInputFlag::AutoPeriodOff |
//                                 TextInputFlag::SpellCheckOff |
//                                 TextInputFlag::WordSubstitutionOff);
    // title bar
    TextFieldTitleBarKindProperties *tbp = new TextFieldTitleBarKindProperties();
    _titleTextField = tbp->textField();
    _titleTextField->setFocusPolicy(FocusPolicy::Touch);
    _titleTextField->setBackgroundVisible(true);
    _titleTextField->input()->setSubmitKey(SubmitKey::Done);
    conn(_titleTextField, SIGNAL(focusedChanged(bool)),
        this, SLOT(onTitleTextFieldFocusChanged(bool)));
    conn(_titleTextField->input(), SIGNAL(submitted(bb::cascades::AbstractTextControl*)),
        this, SLOT(onTitleTextFieldSubmitted()));

    _textArea = TextArea::create()
        .format(TextFormat::Html)
        .addKeyListener(KeyListener::create()
            .onKeyEvent(this, SLOT(onTextAreaKeyEvent(bb::cascades::KeyEvent*))))
        .layoutProperties(StackLayoutProperties::create()
        .spaceQuota(1))
        .bottomMargin(0);
    _progressIndicator = ProgressIndicator::create()
        .vertical(VerticalAlignment::Bottom)
        .topMargin(0);

    // action items and keyboard shortcuts
    _saveAction = ActionItem::create()
        .addShortcut(Shortcut::create().key("s"));
    // TODO: add action items and shortcuts for:
    // * prev/next tab
    // * undo/redo

    // set the final content
    setContent(Page::create()
        .titleBar(TitleBar::create(TitleBarKind::TextField)
            .kindProperties(tbp))
        .content(Container::create()
            .add(_textArea)
            .add(_progressIndicator))
        .addAction(_saveAction, ActionBarPlacement::Signature)
        // TODO: enable the user to choose between the different
        // action bar visibility
        // Visible/Hidden/Compact/(Overlay?)
        // - for hidden: we need to implement a gesture recognize that
        //   enables the user to open the drawer by a right-to-left swipe
        //   (or maybe there is an easier way to turn this directly on in sdk?)
        // - for compact: we need to think about how the signature action
        //   will block the progress bar
        //   one option might be to shift the progress bar to the top
        //   (and stop the visibility hack), which works well too
        .actionBarVisibility(ChromeVisibility::Hidden));

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
            disconn(_textArea, SIGNAL(textChanging(QString)),
                this, SLOT(onTextAreaTextChanged(QString)));
            disconn(_buffer, SIGNAL(nameChanged(QString)),
                this, SLOT(setTitle(QString)));
            disconn(_buffer, SIGNAL(filetypeChanged(QString)),
                this, SLOT(onBufferFiletypeChanged(QString)));
            disconn(_buffer, SIGNAL(contentChanged(QString)),
                this, SLOT(onBufferContentChanged(QString)));
            disconn(_buffer, SIGNAL(inProgressChanged(float)),
                this, SLOT(onBufferProgressChanged(float)));
            disconn(_saveAction, SIGNAL(triggered()),
                _buffer, SLOT(save()));
        }
        _buffer = buffer;
        // set up the initial value for the title
        setTitle(_buffer->name());
        onBufferFiletypeChanged(_buffer->filetype());
        onBufferContentChanged(_buffer->content());
        onBufferProgressChanged(0);
        conn(_textArea, SIGNAL(textChanging(QString)),
            this, SLOT(onTextAreaTextChanged(QString)));
        conn(_buffer, SIGNAL(nameChanged(QString)),
            this, SLOT(setTitle(QString)));
        conn(_buffer, SIGNAL(filetypeChanged(QString)),
            this, SLOT(onBufferFiletypeChanged(QString)));
        conn(_buffer, SIGNAL(contentChanged(QString)),
            this, SLOT(onBufferContentChanged(QString)));
        conn(_buffer, SIGNAL(inProgressChanged(float)),
            this, SLOT(onBufferProgressChanged(float)));
        conn(_saveAction, SIGNAL(triggered()),
            _buffer, SLOT(save()));
    }
}

void View::onTitleTextFieldSubmitted()
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
    _buffer->setContent(text, _textArea->editor()->cursorPosition());
}

void View::onTextAreaKeyEvent(bb::cascades::KeyEvent *event)
{
    if (event->isPressed()) {
        if ((_modPressed & KEYFLAG_RETURN) == KEYFLAG_RETURN) {
            // capture text area only keys
            switch (event->keycap()) {
                case KEYCODE_T:
                    _textArea->editor()->insertPlainText("\t");
                    break;
            }
        } else if (event->duration() >= MOD_THRESHOLD) {
            // if the duration is longer than the specified one
            switch (event->keycap()) {
                case KEYCODE_RETURN:
                    printf("return pressed!\n");
                    _modPressed |= KEYFLAG_RETURN;
                    _textArea->setEditable(false);
                    break;
            }
        }
    } else {
        switch (event->keycap()) {
            case KEYCODE_RETURN:
                if ((_modPressed & KEYFLAG_RETURN) == KEYFLAG_RETURN) {
                    _modPressed ^= KEYFLAG_RETURN;
                    _textArea->setEditable(true);
                }
                break;
        }
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
//        printf("## text area out of sync with buffer content\n### text area: %s\n### buffer: %s\n", qPrintable(_textArea->text()), qPrintable(content));
        int oldPos = _textArea->editor()->cursorPosition();
        _textArea->setText(content);
        _textArea->editor()->setCursorPosition(oldPos);
    }
}

void View::onBufferProgressChanged(float progress)
{
    if (progress <= 0) {
        _progressIndicator->setVisible(false);
    } else {
        _progressIndicator->setVisible(true);
    }
    _progressIndicator->setValue(progress);
}

void View::onLanguageChanged()
{
    _titleTextField->setHintText(tr("Enter the title"));
    _textArea->setHintText(tr("Enter the content"));
    _saveAction->setTitle(tr("Save"));
    if (_buffer) {
        // reset the default name
        setTitle(_buffer->name());
    }
}
