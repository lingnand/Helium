/*
 * View.cpp
 *
 *  Created on: Nov 21, 2014
 *      Author: lingnan
 */

#include <src/View.h>
#include <src/Buffer.h>
#include <src/ModKeyListener.h>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/UIPalette>
#include <bb/cascades/Page>
#include <bb/cascades/Container>
#include <bb/cascades/ImageView>
#include <bb/cascades/Button>
#include <bb/cascades/CheckBox>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/ProgressIndicator>
#include <bb/cascades/TextArea>
#include <bb/cascades/KeyEvent>
#include <bb/cascades/TextStyle>
#include <bb/cascades/TextField>
#include <bb/cascades/TextInputProperties>
#include <bb/cascades/TitleBar>
#include <bb/cascades/TextFieldTitleBarKindProperties>
#include <bb/cascades/FreeFormTitleBarKindProperties>
#include <bb/cascades/TitleBarExpandableArea>
#include <bb/cascades/ActionItem>
#include <bb/cascades/Shortcut>
#include <bb/system/SystemToast>
#include <bb/system/SystemUiPosition>

using namespace bb::cascades;

#define EMPTY_BUFFER_TITLE "No Name"
// keys
#define KEYFLAG_NONE 0
#define KEYFLAG_RETURN 1

View::View(Buffer* buffer):
        _titleField(NULL), _textArea(NULL), _progressIndicator(NULL),
        _buffer(NULL),
        _mode(Normal),
        _modUsed(false)
{
    _textArea = TextArea::create()
        .format(TextFormat::Html)
        .focusPolicy(FocusPolicy::Touch)
        .layoutProperties(StackLayoutProperties::create()
            .spaceQuota(1))
        .bottomMargin(0);
    _textArea->addKeyListener(ModKeyListener::create(KEYCODE_RETURN)
        .onModifiedKeyPressed(this, SLOT(onNormalModeModifiedKeyPressed(bb::cascades::KeyEvent*)))
        .onModKeyPressed(this, SLOT(onTextAreaModKeyPressed(bb::cascades::KeyEvent*)))
        .onTextAreaInputModeChanged(_textArea, SLOT(setInputMode(bb::cascades::TextAreaInputMode::Type)))
        .handleFocusOn(_textArea, SIGNAL(focusedChanged(bool))));

    // TODO: monospace font doesn't play well with italic
    // so what we need to do is to reformat the style file for highlight
//    _textArea->textStyle()->setFontFamily("\"Andale Mono\", monospace");

    // title bar
    TextFieldTitleBarKindProperties *titleBarProperties = new TextFieldTitleBarKindProperties;
    _titleField = titleBarProperties->textField();
    _titleField->setFocusPolicy(FocusPolicy::Touch);
    _titleField->setBackgroundVisible(true);
    _titleField->textStyle()->setBase(SystemDefaults::TextStyles::titleText());
    conn(_titleField, SIGNAL(focusedChanged(bool)),
        this, SLOT(onTitleFieldFocusChanged(bool)));
    _titleField->addKeyListener(ModKeyListener::create(KEYCODE_RETURN)
        .onModifiedKeyPressed(this, SLOT(onNormalModeModifiedKeyPressed(bb::cascades::KeyEvent*)))
        .onModKeyPressed(_textArea, SLOT(requestFocus()))
        .onTextFieldInputModeChanged(_titleField, SLOT(setInputMode(bb::cascades::TextFieldInputMode::Type)))
        .handleFocusOn(_titleField, SIGNAL(focusedChanged(bool))));
    // the default titlebar
    _titleBar = TitleBar::create(TitleBarKind::TextField)
        .kindProperties(titleBarProperties);

    _progressIndicator = ProgressIndicator::create()
        .vertical(VerticalAlignment::Bottom)
        .topMargin(0);

    // the candidate titlebars
    // the search and replace titlebar
    _findField = TextField::create().vertical(VerticalAlignment::Center)
        .layoutProperties(StackLayoutProperties::create().spaceQuota(1));
    _replaceField = TextField::create().vertical(VerticalAlignment::Center)
        .layoutProperties(StackLayoutProperties::create().spaceQuota(1))
        .submitKey(SubmitKey::Search)
        .onSubmitted(this, SLOT(findNext()));
    _findCaseSensitiveCheckBox = new CheckBox;
    FreeFormTitleBarKindProperties *findTitleBarProperties = FreeFormTitleBarKindProperties::create()
        .expandableIndicator(TitleBarExpandableAreaIndicatorVisibility::Hidden)
        .content(Container::create()
            .layout(StackLayout::create().orientation(LayoutOrientation::LeftToRight))
            .left(20).right(20)
            .add(ImageView::create().vertical(VerticalAlignment::Center)
                .imageSource((QUrl("asset:///images/ic_search.png")))
                .filterColor(SystemDefaults::Paints::defaultText()))
            .add(_findField)
            .add(_replaceField)
            .add(Button::create().vertical(VerticalAlignment::Center)
                .imageSource(QUrl("asset:///images/ic_sort_black.png"))
                .preferredWidth(0)
                .onClicked(this, SLOT(onFindOptionButtonClicked()))))
        .expandableContent(Container::create()
                .background(_titleBar->ui()->palette()->plain())
                .bottom(5)
                .add(Container::create()
                    .background(Color::White)
                    .top(20).bottom(20).left(30).right(30)
                    .add(_findCaseSensitiveCheckBox)));
    _findExpandableArea = findTitleBarProperties->expandableArea();
    _findTitleBar = TitleBar::create(TitleBarKind::FreeForm)
        .kindProperties(findTitleBarProperties);

    // the page
    _page = Page::create()
        .titleBar(_titleBar)
        .content(Container::create()
            .add(_textArea)
            .add(_progressIndicator))
        // TODO: enable the user to choose between the different
        // action bar visibility
        // Visible/Hidden
        // - for hidden: we need to implement a gesture recognize that
        //   enables the user to open the drawer by a right-to-left swipe
        //   (or maybe there is an easier way to turn this directly on in sdk?)
        //   one option might be to shift the progress bar to the top
        //   (and stop the visibility hack), which works well too
        .actionBarVisibility(ChromeVisibility::Visible)
        // shortcuts
        .addShortcut(Shortcut::create().key("Enter")
            .onTriggered(this, SLOT(autoFocus())));

    setNormalModeActions();

    // set the final content
    setContent(_page);

    // label initial load
    onLanguageChanged();

    // set the buffer
    setBuffer(buffer);
}

/* miscellaneous actions */
void View::onOutOfView()
{
    findModeOff();
}

void View::autoFocus()
{
    switch (_mode) {
        case Normal:
            // focus title text only when the text area is empty
            if (!_buffer->hasPlainText())
                _titleField->requestFocus();
            else
                _textArea->requestFocus();
            break;
        case Find:
            _findField->requestFocus();
            break;
    }
}

void View::setNormalModeActions()
{
    _page->removeAllActions();
    _saveAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_save.png"))
        .addShortcut(Shortcut::create().key("s"))
        .onTriggered(this, SLOT(onSaveTriggered()));
    // * undo/redo
    _undoAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_undo.png"))
        .addShortcut(Shortcut::create().key("z"))
        .onTriggered(this, SLOT(onUndoTriggered()));
    _redoAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_redo.png"))
        .addShortcut(Shortcut::create().key("x"))
        .onTriggered(this, SLOT(onRedoTriggered()));
    // TODO: add action items and shortcuts for:
    // * search
    _findAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_search.png"))
        .addShortcut(Shortcut::create().key("f"))
        .onTriggered(this, SLOT(findModeOn()));
    reloadNormalModeActionTitles();
    _page->addAction(_saveAction, ActionBarPlacement::Signature);
    _page->addAction(_undoAction, ActionBarPlacement::OnBar);
    _page->addAction(_redoAction, ActionBarPlacement::OnBar);
    _page->addAction(_findAction);
}

void View::setFindModeActions()
{
    _page->removeAllActions();
    _findPrevAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_backward.png"))
        .addShortcut(Shortcut::create().key("p"))
        .onTriggered(this, SLOT(findPrev()));
    _findNextAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_forward.png"))
        .addShortcut(Shortcut::create().key("n"))
        .onTriggered(this, SLOT(findNext()));
    _replaceNextAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_rename.png"))
        .addShortcut(Shortcut::create().key("r"))
        .onTriggered(this, SLOT(replaceNext()));
    _replaceAllAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_rename.png"))
        .addShortcut(Shortcut::create().key("Shift+r"))
        .onTriggered(this, SLOT(replaceAll()));
    _findCancelAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_cancel.png"))
        .addShortcut(Shortcut::create().key("q"))
        .onTriggered(this, SLOT(findModeOff()));
    reloadFindModeActionTitles();
    _page->addAction(_findPrevAction);
    _page->addAction(_findNextAction, ActionBarPlacement::Signature);
    _page->addAction(_replaceNextAction, ActionBarPlacement::OnBar);
    _page->addAction(_replaceAllAction);
    _page->addAction(_findCancelAction, ActionBarPlacement::OnBar);
}

// TODO: make it such that when the user touches the textarea, quit the find
// mode automatically?
// also, when the user taps away the cursor location, we should make our search
// start with that as well
// TODO: further, on each new search due to change in the search field, we should
// always stick to using the same old starting cursor location
bool View::findModeOn()
{
    _mode = Find;
    _page->setTitleBar(_findTitleBar);
    setFindModeActions();
    _textArea->setEditable(false);
    _findField->resetText();
    _replaceField->resetText();
    // states
    _findBuffer.clear();
    _findQuery.clear();
    _findOffset = _textArea->editor()->cursorPosition();
    // focus
    _findField->requestFocus();
    return true;
}

void View::select(const TextSelection &selection)
{
    _textArea->editor()->setSelection(selection.first, selection.second);
}

void View::findNext()
{
    // check if we need to set up the iterator
    QString find = _findField->text();
    if (find.isEmpty()) {
        printf("please enter text to search!\n");
        return;
    }
    if (find != _findQuery) {
        _findQuery = find;
        if (_findBuffer.empty())
            _findBuffer = _buffer->plainText().toUtf8().constData();
        if (_findOffset == 0) {
            _lastFindLoop = true;
            _bofIndex = 0;
        } else {
            _lastFindLoop = false;
        }
        _findRegex = boost::regex(std::string(find.toUtf8().constData()));
        _findIterator = boost::sregex_iterator(
                _findBuffer.begin() + _findOffset,
                _findBuffer.end(),
                _findRegex);
        _findComplete = false;
        _findHits.clear();
        _findIndex = 0;
    } else {
        if (_findComplete && _findHits.isEmpty()) {
            printf("not found!\n");
            return;
        }
        // check if there is something on findIndex
        ++_findIndex;
        if (_findComplete && _findIndex == _findHits.count()) {
            _findIndex = 0;
        }
        if (_lastFindLoop && _findIndex == _bofIndex) {
            printf("reached end of document, beginning from top!\n");
        }
        if (_findIndex < _findHits.count()) {
            select(_findHits[_findIndex]);
            return;
        }
        // we've reached end of the hit list
        _findIterator++;
    }
    boost::sregex_iterator end = boost::sregex_iterator();
    if (_findIterator == end) {
        if (_lastFindLoop) {
            printf("reached end of the last find loop; marking complete\n");
            _findComplete = true;
            _findIndex = 0;
            Q_ASSERT(!_findHits.isEmpty());
            select(_findHits[0]);
            return;
        } else {
            _lastFindLoop = true;
            // we've reached end
            // try from beginning again
            _findIterator = boost::sregex_iterator(
                    _findBuffer.begin(),
                    _findBuffer.end(),
                    _findRegex);
            if (_findIterator == end) {
                printf("not found!\n");
                Q_ASSERT(_findHits.isEmpty());
                _findComplete = true;
                return;
            }
            _bofIndex = _findIndex;
            printf("reached end of document, continue from top!\n");
        }
    }
    // found something
    // decide whether we need to insert the location into the list
    printf("current findIndex: %d, findhits count: %d\n", _findIndex, _findHits.count());
    Q_ASSERT(_findIndex == _findHits.count());
    TextSelection selection((*_findIterator)[0].first - _findBuffer.begin(),
            (*_findIterator)[0].second - _findBuffer.begin());
    if (!_findHits.isEmpty() && selection == _findHits[0]) {
        // we've wrapped around and found the same selection as the first one
        printf("found the same match in the list; marking complete\n");
        _findIndex = 0;
        _findComplete = true;
    } else {
        _findHits.append(selection);
    }
    select(selection);
}

void View::findPrev()
{

}

void View::replaceNext()
{

}

void View::replaceAll()
{

}

bool View::findModeOff()
{
    if (_mode == Find) {
        _page->setTitleBar(_titleBar);
        setNormalModeActions();
        _mode = Normal;
        _textArea->setEditable(true);
        _textArea->requestFocus();
        return true;
    }
    return false;
}

void View::setTitle(const QString& title)
{
    if (title.isEmpty()) {
        Tab::setTitle(tr(EMPTY_BUFFER_TITLE));
    } else {
        _titleField->setText(title);
        Tab::setTitle(title);
    }
}

void View::setBuffer(Buffer* buffer)
{
    if (buffer != _buffer) {
        // we only deal with real buffers, can't set to NULL
        Q_ASSERT(buffer);
        if (_buffer) {
            disconn(_textArea, SIGNAL(textChanging(QString)),
                this, SLOT(onTextAreaTextChanged(QString)));
            disconn(_buffer, SIGNAL(contentChanged(QString, int)),
                this, SLOT(onBufferContentChanged(QString, int)));
            disconn(_buffer, SIGNAL(nameChanged(QString)),
                this, SLOT(setTitle(QString)));
            disconn(_buffer, SIGNAL(filetypeChanged(QString)),
                this, SLOT(onBufferFiletypeChanged(QString)));
            disconn(_buffer, SIGNAL(inProgressChanged(float)),
                this, SLOT(onBufferProgressChanged(float)));
            disconn(_buffer, SIGNAL(hasUndosChanged(bool)),
                _undoAction, SLOT(setEnabled(bool)));
            disconn(_buffer, SIGNAL(hasRedosChanged(bool)),
                _redoAction, SLOT(setEnabled(bool)));
        }
        _buffer = buffer;
        onBufferContentChanged(_buffer->content(), -1);
        conn(_textArea, SIGNAL(textChanging(QString)),
            this, SLOT(onTextAreaTextChanged(QString)));
        conn(_buffer, SIGNAL(contentChanged(QString, int)),
            this, SLOT(onBufferContentChanged(QString, int)));

        setTitle(_buffer->name());
        conn(_buffer, SIGNAL(nameChanged(QString)),
            this, SLOT(setTitle(QString)));

        onBufferFiletypeChanged(_buffer->filetype());
        conn(_buffer, SIGNAL(filetypeChanged(QString)),
            this, SLOT(onBufferFiletypeChanged(QString)));

        onBufferProgressChanged(0);
        conn(_buffer, SIGNAL(inProgressChanged(float)),
            this, SLOT(onBufferProgressChanged(float)));

        _undoAction->setEnabled(_buffer->hasUndo());
        conn(_buffer, SIGNAL(hasUndosChanged(bool)),
            this, SLOT(onBufferHasUndosChanged(bool)));

        _redoAction->setEnabled(_buffer->hasRedo());
        conn(_buffer, SIGNAL(hasRedosChanged(bool)),
            this, SLOT(onBufferHasRedosChanged(bool)));
    }
}

void View::onFindOptionButtonClicked()
{
    _findExpandableArea->setExpanded(!_findExpandableArea->isExpanded());
}

void View::onTitleFieldFocusChanged(bool focus)
{
    if (!focus) {
        // the user defocused the text field
        // set the buffer name
        _buffer->setName(_titleField->text());
    }
}

void View::onTextAreaTextChanged(const QString& text)
{
//    fprintf(stdout, "text changed: %s\n", qPrintable(text));
//    if (_textArea->editor()->selectedText().isEmpty())
    // only when the the cursor is currently before
    _buffer->setContent(text, _textArea->editor()->cursorPosition());
}

void View::onModPressTimeout() {
}

void View::onTextAreaModKeyPressed(bb::cascades::KeyEvent *event)
{
    if (_mode == Normal)
        _textArea->editor()->insertPlainText(event->unicode());
}

void View::onNormalModeModifiedKeyPressed(bb::cascades::KeyEvent *event)
{
    if (_mode == Normal) {
        switch (event->keycap()) {
            case KEYCODE_T:
                _textArea->editor()->insertPlainText("\t");
                break;
            case KEYCODE_S:
                onSaveTriggered();
                break;
            case KEYCODE_Z:
                onUndoTriggered();
                break;
            case KEYCODE_X:
                onRedoTriggered();
                break;
            case KEYCODE_F:
                findModeOn();
                break;
            // TODO: add shortcut for open and new file
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

void View::onBufferContentChanged(const QString& content, int cursorPosition) {
    if (_textArea->text() != content) {
//        printf("## text area out of sync with buffer content\n### text area: %s\n### buffer: %s\n", qPrintable(_textArea->text()), qPrintable(content));
        int pos = cursorPosition < 0 ? _textArea->editor()->cursorPosition() : cursorPosition;
        _textArea->setText(content);
        _textArea->editor()->setCursorPosition(pos);
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

void View::onBufferHasUndosChanged(bool hasUndos)
{
    _undoAction->setEnabled(hasUndos);
}

void View::onBufferHasRedosChanged(bool hasRedos)
{
    _redoAction->setEnabled(hasRedos);
}

void View::onSaveTriggered()
{
    _buffer->save();
}

void View::onUndoTriggered()
{
    _buffer->undo();
    _textArea->requestFocus();
}

void View::onRedoTriggered()
{
    _buffer->redo();
    _textArea->requestFocus();
}

void View::reloadNormalModeActionTitles()
{
    _saveAction->setTitle(tr("Save"));
    _undoAction->setTitle(tr("Undo"));
    _redoAction->setTitle(tr("Redo"));
    _findAction->setTitle(tr("Find"));
}

void View::reloadFindModeActionTitles()
{
    _findPrevAction->setTitle(tr("Find Previous"));
    _findNextAction->setTitle(tr("Find next"));
    _replaceNextAction->setTitle(tr("Replace next"));
    _replaceAllAction->setTitle(tr("Replace all remaining"));
    _findCancelAction->setTitle(tr("Cancel"));
}

void View::onLanguageChanged()
{
    _titleField->setHintText(tr("Enter the title"));
    _textArea->setHintText(tr("Enter the content"));

    _findCaseSensitiveCheckBox->setText("Case sensitive");
    _findField->setHintText(tr("Find text"));
    _replaceField->setHintText(tr("Replace with"));

    // actions
    switch (_mode) {
        case Normal:
            reloadNormalModeActionTitles();
            break;
        case Find:
            reloadFindModeActionTitles();
            break;
    }

    if (_buffer) {
        // reset the default name
        setTitle(_buffer->name());
    }
}
