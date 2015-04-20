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
#include <bb/system/Clipboard>

using namespace bb::cascades;

#define BUFFER_TITLE_EMPTY "No Name"
// keys
#define KEYFLAG_NONE 0
#define KEYFLAG_RETURN 1
// strings
#define HINT_TEXT_TITLE_FIELD "Enter the title"
#define HINT_TEXT_TEXT_AREA "Enter the content"

#define ACTION_TITLE_SAVE "Save"
#define ACTION_TITLE_UNDO "Undo"
#define ACTION_TITLE_REDO "Redo"
#define ACTION_TITLE_FIND "Find"

#define ACTION_TITLE_GO_TO_FIND_FIELD "Go to find field"
#define ACTION_TITLE_FIND_PREV "Find previous"
#define ACTION_TITLE_FIND_NEXT "Find next"
#define ACTION_TITLE_REPLACE_NEXT "Replace next"
#define ACTION_TITLE_REPLACE_ALL "Replace all remaining"
#define ACTION_TITLE_FIND_CANCEL "Cancel"

#define HINT_TEXT_FIND_FIELD "Find text"
#define HINT_TEXT_REPLACE_FIELD "Replace with"
#define CHECKBOX_TEXT_FIND_CASE_SENSITIVE "Case sensitive"
// messages
#define TOAST_FIND_INVALID_QUERY_EMPTY "Search query can't be empty!"
#define TOAST_FIND_NOT_FOUND "Not found"
#define TOAST_FIND_REACHED_END "Reached end of document, beginning from top!"
#define TOAST_FIND_REACHED_TOP "Reached top of document, beginning from bottom!"
// dialog
#define DIALOG_REPLACE_FROM_TOP_CONFIRM "Yes"
#define DIALOG_REPLACE_FROM_TOP_CANCEL "No"
#define DIALOG_REPLACE_FROM_TOP_TITLE "End of file reached"
#define DIALOG_REPLACE_FROM_TOP_BODY "%1 replacement(s) were made. Do you want to continue from the beginning?"
#define DIALOG_REPLACE_FINISHED_CONFIRM "OK"
#define DIALOG_REPLACE_FINISHED_TITLE "Replace finished"
#define DIALOG_REPLACE_FINISHED_BODY "%1 replacement(s) have been made."

// the one side range for partial highlight (will be put into settings)
#define PARTIAL_HIGHLIGHT_RANGE 20

// #### View
// TODO: it would be good if you can modulize the undo/redo functionality
// and then attach it to each textfield/area like what you did with the modkeylistener!
// that would be awesome...
View::View(Buffer* buffer):
        _titleField(NULL), _textArea(NULL), _progressIndicator(NULL),
        _buffer(NULL),
        _mode(Normal),
        _findBufferDirty(true), _modifyingTextArea(false)
{
    _textArea = TextArea::create()
        .format(TextFormat::Html)
        .inputFlags(TextInputFlag::SpellCheckOff
                    | TextInputFlag::AutoCorrectionOff
                    | TextInputFlag::AutoCapitalizationOff
                    | TextInputFlag::AutoPeriodOff)
        .focusPolicy(FocusPolicy::Touch)
        .layoutProperties(StackLayoutProperties::create()
            .spaceQuota(1))
        .bottomMargin(0);
    _textAreaModKeyListener = ModKeyListener::create(KEYCODE_RETURN)
        .onModifiedKeyPressed(this, SLOT(onTextAreaModifiedKeyPressed(bb::cascades::KeyEvent*)))
        .onModKeyPressed(this, SLOT(onTextAreaModKeyPressed(bb::cascades::KeyEvent*)))
        .onTextAreaInputModeChanged(_textArea, SLOT(setInputMode(bb::cascades::TextAreaInputMode::Type)))
        .handleFocusOn(_textArea, SIGNAL(focusedChanged(bool)));
    _textArea->addKeyListener(_textAreaModKeyListener);
    conn(_textArea->editor(), SIGNAL(cursorPositionChanged(int)),
            this, SLOT(onTextAreaCursorPositionChanged(int)));
    // setup the timer for partial highlight update
    _partialHighlightUpdateTimer.setSingleShot(true);
    conn(&_partialHighlightUpdateTimer, SIGNAL(timeout()),
            this, SLOT(updateTextAreaPartialHighlight()));

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
        .onModifiedKeyPressed(this, SLOT(onTitleFieldModifiedKeyPressed(bb::cascades::KeyEvent*)))
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
        .focusPolicy(FocusPolicy::Touch)
        .layoutProperties(StackLayoutProperties::create().spaceQuota(1));
    _replaceField = TextField::create().vertical(VerticalAlignment::Center)
        .focusPolicy(FocusPolicy::Touch)
        .layoutProperties(StackLayoutProperties::create().spaceQuota(1));
    // set up the find fields mod keys
    // TODO: not only focus the replaceField, but also select the existing content of the replace field
    _findField->addKeyListener(ModKeyListener::create(KEYCODE_RETURN)
        .onModifiedKeyPressed(this, SLOT(onFindFieldModifiedKeyPressed(bb::cascades::KeyEvent*)))
        .onModKeyPressed(_replaceField, SLOT(requestFocus()))
        .onTextFieldInputModeChanged(_findField, SLOT(setInputMode(bb::cascades::TextFieldInputMode::Type)))
        .handleFocusOn(_findField, SIGNAL(focusedChanged(bool))));
    _replaceField->addKeyListener(ModKeyListener::create(KEYCODE_RETURN)
        .onModifiedKeyPressed(this, SLOT(onReplaceFieldModifiedKeyPressed(bb::cascades::KeyEvent*)))
        .onModKeyPressed(this, SLOT(findNext()))
        .onTextFieldInputModeChanged(_replaceField, SLOT(setInputMode(bb::cascades::TextFieldInputMode::Type)))
        .handleFocusOn(_replaceField, SIGNAL(focusedChanged(bool))));
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
            if (_buffer->state().empty())
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
    _goToFindFieldAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_search.png"))
        .addShortcut(Shortcut::create().key("f"))
        .onTriggered(_findField, SLOT(requestFocus()));
    _findPrevAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_backward.png"))
        .addShortcut(Shortcut::create().key("p"))
        .onTriggered(this, SLOT(findPrev()));
    // TODO: we should probably add the "enter" shortcut?
    // (but for some reason it's not working properly)
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
        .addShortcut(Shortcut::create().key("a"))
        .onTriggered(this, SLOT(replaceAll()));
    _findCancelAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_cancel.png"))
        .addShortcut(Shortcut::create().key("q"))
        .onTriggered(this, SLOT(findModeOff()));
    _undoAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_undo.png"))
        .addShortcut(Shortcut::create().key("z"))
        .onTriggered(this, SLOT(onUndoTriggered()));
    _redoAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_redo.png"))
        .addShortcut(Shortcut::create().key("x"))
        .onTriggered(this, SLOT(onRedoTriggered()));
    reloadFindModeActionTitles();
    _page->addAction(_goToFindFieldAction);
    _page->addAction(_findPrevAction);
    _page->addAction(_findNextAction, ActionBarPlacement::Signature);
    _page->addAction(_replaceNextAction, ActionBarPlacement::OnBar);
    _page->addAction(_replaceAllAction);
    _page->addAction(_findCancelAction, ActionBarPlacement::OnBar);
    _page->addAction(_undoAction);
    _page->addAction(_redoAction);
}

bool View::findModeOn()
{
    if (_mode != Find) {
        _mode = Find;
        // disable current key handler
        _textAreaModKeyListener->setEnabled(false);
        // titlebar
        _page->setTitleBar(_findTitleBar);
        // actionbar
        setFindModeActions();
        // focus
        _findField->requestFocus();
        _textArea->setEditable(false);
        return true;
    }
    return false;
}

bool View::findModeOff()
{
    if (_mode == Find) {
        // clear states
        _findBuffer.clear();
        _findQuery.clear();
        _findBufferDirty = true;

        _page->setTitleBar(_titleBar);
        setNormalModeActions();
        _mode = Normal;
        _textArea->loseFocus();
        _textArea->setEditable(true);
        _textAreaModKeyListener->setEnabled(true);
//        _textArea->editor()->setCursorPosition(_textArea->editor()->cursorPosition());
        _textArea->requestFocus();
        return true;
    }
    return false;
}

void View::select(const TextSelection &selection)
{
    // use lose focus and then refocus to force scrolling to the right position
    _textArea->loseFocus();
    _textArea->editor()->setSelection(selection.start, selection.end);
    _textArea->requestFocus();
}

// this function is responsible for updating the find query/regex, if any change has taken place
View::FindQueryUpdateStatus View::updateFindQuery(bool interactive)
{
    QString find = _findField->text();
    if (find.isEmpty()) {
        if (interactive)
            toast(tr(TOAST_FIND_INVALID_QUERY_EMPTY));
        _findComplete = true;
        return Invalid;
    }
    FindQueryUpdateStatus status = Unchanged;
    if (find != _findQuery) {
        _findQuery = find;
        _findRegex = boost::wregex(find.toStdWString());
        status = Changed;
    }
    if (_findBufferDirty) {
        _findBufferDirty = false;
        QString output;
        QTextStream stream(&output);
        _buffer->state().writePlainText(stream);
        stream.flush();
        _findBuffer = output.toStdWString();
        status = Changed;
    }
    if (status == Changed)
        _findComplete = false;
    return status;
}

void View::findNext()
{
    findNextWithOptions(true, updateFindQuery(true));
}

void View::findNextWithOptions(bool interactive, FindQueryUpdateStatus status)
{
    _textArea->requestFocus();
    // check if we need to set up the iterator
    switch (status) {
        case Invalid:
            return;
        case Changed: {
            int findOffset = _textArea->editor()->cursorPosition();
            if (findOffset == 0) {
                _bofIndex = 0;
            } else {
                _bofIndex = -1;
            }
            _findIterator = boost::wsregex_iterator(
                    _findBuffer.begin() + findOffset,
                    _findBuffer.end(),
                    _findRegex);
            _findHits.clear();
            _findIndex = 0;
            break;
        }
        case Unchanged: {
            if (_findComplete && _findHits.isEmpty()) {
                if (interactive)
                    toast(tr(TOAST_FIND_NOT_FOUND));
                return;
            }
            // check if there is something on findIndex
            ++_findIndex;
            if (_findComplete && _findIndex == _findHits.count()) {
                _findIndex = 0;
            }
            if (_findIndex == _bofIndex && interactive) {
                toast(tr(TOAST_FIND_REACHED_END));
            }
            if (_findIndex < _findHits.count()) {
                if (interactive)
                    select(_findHits[_findIndex].selection);
                return;
            }
            // we've reached end of the hit list
            _findIterator++;
            break;
        }
    }
    boost::wsregex_iterator end = boost::wsregex_iterator();
    if (_findIterator == end) {
        if (_bofIndex >= 0) {
            qDebug() << "reached end of the last find loop; marking complete";
            _findComplete = true;
            _findIndex = 0;
            if(_findHits.isEmpty()) {
                if (interactive)
                    toast(tr(TOAST_FIND_NOT_FOUND));
            } else {
                if (interactive)
                    select(_findHits[0].selection);
            }
            return;
        } else {
            // we've reached end
            // try from beginning again
            _findIterator = boost::wsregex_iterator(
                    _findBuffer.begin(),
                    _findBuffer.end(),
                    _findRegex);
            if (_findIterator == end) {
                if (interactive)
                    toast(tr(TOAST_FIND_NOT_FOUND));
                Q_ASSERT(_findHits.isEmpty());
                _findComplete = true;
                return;
            }
            _bofIndex = _findIndex;
            if (interactive)
                toast(tr(TOAST_FIND_REACHED_END));
        }
    }
    // found something
    // decide whether we need to insert the location into the list
    qDebug() << "current findIndex:" << _findIndex << "findhits count:" << _findHits.count();
    Q_ASSERT(_findIndex == _findHits.count());
    TextSelection selection((*_findIterator)[0].first - _findBuffer.begin(),
            (*_findIterator)[0].second - _findBuffer.begin());
    if (!_findHits.isEmpty() && selection == _findHits[0].selection) {
        // we've wrapped around and found the same selection as the first one
        qDebug() << "found the same match in the list; marking complete";
        // take care of the bofIndex to match the change in the findIndex
        if (_bofIndex == _findIndex)
            _bofIndex = 0;
        _findIndex = 0;
        _findComplete = true;
    } else {
        _findHits.append(FindMatch(selection, *_findIterator));
    }
    if (interactive)
        select(selection);
}

void View::findPrev()
{
    _textArea->requestFocus();
    switch (updateFindQuery(true)) {
        case Invalid:
            return;
        case Changed: {
            _textArea->requestFocus();
            int findOffset = _textArea->editor()->cursorPosition();
            // we always enter the last find loop, because a reverse search will trigger
            // a whole run-down iterator
            _bofIndex = 0;
            _findIterator = boost::wsregex_iterator(
                    _findBuffer.begin(),
                    _findBuffer.end(),
                    _findRegex);
            _findHits.clear();
            // use -1 to indicate nothing in the hit list
            _findIndex = -1;
            // begin to find all the matches until passes the given offset
            boost::wsregex_iterator end = boost::wsregex_iterator();
            while (true) {
                if (_findIterator == end) {
                    // no more matches can be found
                    _findComplete = true;
                    break;
                }
                ++_findIndex;
                // first append the result
                _findHits.append(FindMatch(
                        TextSelection((*_findIterator)[0].first - _findBuffer.begin(),
                        (*_findIterator)[0].second - _findBuffer.begin())
                        , *_findIterator));
                if ((*_findIterator)[0].first - _findBuffer.begin() > findOffset) {
                    if (_findIndex < 1) {
                        // if this is the first find index, we don't have anything before
                        // the current cursor
                        toast(tr(TOAST_FIND_REACHED_TOP));
                    } else {
                        // let's rewind by one
                        --_findIndex;
                        break;
                    }
                }
                _findIterator++;
            }
            // now hopefully we've settled down on the correct index
            if (_findIndex < 0) {
                Q_ASSERT(_findHits.isEmpty());
                toast(tr(TOAST_FIND_NOT_FOUND));
            } else {
                select(_findHits[_findIndex].selection);
            }
            break;
        }
        case Unchanged: {
            _textArea->requestFocus();
            if (_findComplete && _findHits.isEmpty()) {
                toast(tr(TOAST_FIND_NOT_FOUND));
                return;
            }
            if (_findIndex == _bofIndex) {
                toast(tr(TOAST_FIND_REACHED_TOP));
            }
            // check if there is something on findIndex
            --_findIndex;
            if (_findComplete && _findIndex < 0) {
                _findIndex = _findHits.count() - 1;
            }
            if (_findIndex >= 0) {
                select(_findHits[_findIndex].selection);
                return;
            }
            // we've reached the left end of the hit list
            // findIndex is negative here
            // exhaust the current iterator and switch to the beginning-from-the-top iterator
            // and exhaust that as well. the point is to complete the whole list of find hits
            boost::wsregex_iterator end = boost::wsregex_iterator();
            while (true) {
                // increment find iterator
                _findIterator++;
                if (_findIterator == end) {
                    // no more matches can be found
                    if (_bofIndex < 0) {
                        _findIterator = boost::wsregex_iterator(
                                _findBuffer.begin(),
                                _findBuffer.end(),
                                _findRegex);
                        if (_findIterator == end) {
                            toast(tr(TOAST_FIND_NOT_FOUND));
                            _findComplete = true;
                            break;
                        }
                        _bofIndex = _findHits.count();
                    } else {
                        _findComplete = true;
                        break;
                    }
                }
                // check for wrapping condition
                TextSelection selection((*_findIterator)[0].first - _findBuffer.begin(),
                    (*_findIterator)[0].second - _findBuffer.begin());
                if (!_findHits.isEmpty() && selection == _findHits[0].selection) {
                    _findComplete = true;
                    if (_bofIndex == _findHits.count()) {
                        _bofIndex = 0;
                        toast(tr(TOAST_FIND_REACHED_TOP));
                    }
                    break;
                } else {
                    _findHits.append(FindMatch(selection, *_findIterator));
                }
            }
            _findIndex = _findHits.count() - 1;
            select(_findHits[_findIndex].selection);
            break;
        }
    }
}

void View::replaceNext()
{
    FindQueryUpdateStatus status = updateFindQuery(true);
    if (status == Invalid)
        return;
    if (status == Unchanged) {
        TextSelection current(_textArea->editor()->selectionStart(),
                _textArea->editor()->selectionEnd());
        if (_findIndex >= 0 && _findIndex < _findHits.count() && current == _findHits[_findIndex].selection) {
            // TODO: should we set the cursor to be just after the replacement?
            _buffer->parseReplacement(Replacement(current,
                    QString::fromStdWString(_findHits[_findIndex].match.format(_replaceField->text().toStdWString()))));
            // TODO: modify the findbuffer appropriately instead of doing plainText() again in the next findNext call
            _findBufferDirty = true;
            // there shouldn't be any interactivity here
            status = updateFindQuery(true);
        }
    }
    findNextWithOptions(true, status);
}

// TODO: this appears to be working, but not enough testing at the moment
void View::replaceAll()
{
    FindQueryUpdateStatus status = updateFindQuery(true);
    if (status == Invalid)
        return;
    findNextWithOptions(false, status);
    // get the replace index
    int replaceIndex = _findIndex;
    qDebug() << "replace index is" << replaceIndex;
    QStdWString replacement = _replaceField->text().toStdWString();
    // refill all the findMatches in the _findHits
    while (!_findComplete) {
        // there shouldn't be any change in the status
        findNextWithOptions(false, Unchanged);
    }
    _replaces.clear();
    int i = 0;
    int index;
    // now take from the replaceIndex to bofIndex - 1
    for (; i < _findHits.count(); i++) {
        index = (replaceIndex + i) % _findHits.count();
//        qDebug() << "adding index" << index << "to the first replaces queue";
        if (index == _bofIndex) {
            break;
        }
        _replaces.append(Replacement(_findHits[index].selection,
                QString::fromStdWString(_findHits[index].match.format(replacement))));
    }
    // do the actual replaces
    _buffer->parseReplacement(_replaces);
    _numberOfReplacesTillBottom = _replaces.count();
    _replaces.clear();
    for (; i < _findHits.count(); i++) {
        index = (replaceIndex + i) % _findHits.count();
//        qDebug() << "adding index" << index << "to the second replaces queue";
        _replaces.append(Replacement(_findHits[index].selection,
                QString::fromStdWString(_findHits[index].match.format(replacement))));
    }
    // mark the buffer as dirty
    _findBufferDirty = true;
    dialog(tr(DIALOG_REPLACE_FROM_TOP_CONFIRM), tr(DIALOG_REPLACE_FROM_TOP_CANCEL),
            tr(DIALOG_REPLACE_FROM_TOP_TITLE),
            QString(tr(DIALOG_REPLACE_FROM_TOP_BODY)).arg(_numberOfReplacesTillBottom),
            this, SLOT(onReplaceFromTopDialogFinished(bb::system::SystemUiResult::Type)));
}

void View::onReplaceFromTopDialogFinished(bb::system::SystemUiResult::Type type)
{
    if (type == bb::system::SystemUiResult::ConfirmButtonSelection) {
        _buffer->parseReplacement(_replaces);
        dialog(tr(DIALOG_REPLACE_FINISHED_CONFIRM),
                tr(DIALOG_REPLACE_FINISHED_TITLE),
                QString(tr(DIALOG_REPLACE_FINISHED_BODY))
                    .arg(_numberOfReplacesTillBottom+_replaces.count()));
    }
    _replaces.clear();
}

void View::setTitle(const QString& title)
{
    if (title.isEmpty()) {
        Tab::setTitle(tr(BUFFER_TITLE_EMPTY));
    } else {
        _titleField->setText(title);
        Tab::setTitle(title);
    }
}

void View::setBuffer(Buffer* buffer)
{
    if (buffer != _buffer) {
        findModeOff();
        // we only deal with real buffers, can't set to NULL
        Q_ASSERT(buffer);
        if (_buffer) {
            disconn(_textArea, SIGNAL(textChanging(QString)),
                this, SLOT(onTextAreaTextChanged(QString)));
            disconn(_buffer, SIGNAL(stateChanged(BufferState &, View *, bool)),
                this, SLOT(onBufferStateChanged(BufferState &, View *, bool)));
            disconn(_buffer, SIGNAL(nameChanged(QString)),
                this, SLOT(setTitle(QString)));
            disconn(_buffer, SIGNAL(filetypeChanged(QString)),
                this, SLOT(onBufferFiletypeChanged(QString)));
            disconn(_buffer, SIGNAL(inProgressChanged(float)),
                this, SLOT(onBufferProgressChanged(float)));
            disconn(_buffer, SIGNAL(lockedChanged(bool)),
                this, SLOT(onBufferLockedChanged(bool)));
            disconn(_buffer, SIGNAL(hasUndosChanged(bool)),
                this, SLOT(onBufferHasUndosChanged(bool)));
            disconn(_buffer, SIGNAL(hasRedosChanged(bool)),
                this, SLOT(onBufferHasRedosChanged(bool)));
        }
        _buffer = buffer;
        onBufferStateChanged(buffer->state(), NULL, false);
        conn(_textArea, SIGNAL(textChanging(QString)),
            this, SLOT(onTextAreaTextChanged(QString)));
        conn(_buffer, SIGNAL(stateChanged(BufferState &, View *, bool)),
            this, SLOT(onBufferStateChanged(BufferState &, View *, bool)));

        setTitle(_buffer->name());
        conn(_buffer, SIGNAL(nameChanged(QString)),
            this, SLOT(setTitle(QString)));

        onBufferFiletypeChanged(_buffer->filetype());
        conn(_buffer, SIGNAL(filetypeChanged(QString)),
            this, SLOT(onBufferFiletypeChanged(QString)));

        onBufferProgressChanged(0);
        conn(_buffer, SIGNAL(inProgressChanged(float)),
            this, SLOT(onBufferProgressChanged(float)));

        onBufferLockedChanged(buffer->locked());
        conn(_buffer, SIGNAL(lockedChanged(bool)),
            this, SLOT(onBufferLockedChanged(bool)));

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

void View::onFindFieldModifiedKeyPressed(bb::cascades::KeyEvent *event)
{
    onFindFieldsModifiedKeyPressed(_findField->editor(), event);
}

void View::onReplaceFieldModifiedKeyPressed(bb::cascades::KeyEvent *event)
{
    onFindFieldsModifiedKeyPressed(_replaceField->editor(), event);
}

// TODO: we should probably enable undo and redo
// TODO: we should probably stop those tab related action (open file, new buffer)
void View::onFindFieldsModifiedKeyPressed(bb::cascades::TextEditor *editor, bb::cascades::KeyEvent *event)
{
    switch (event->keycap()) {
        case KEYCODE_T:
            editor->insertPlainText("\t");
            break;
        case KEYCODE_F:
            _findField->requestFocus();
            break;
        case KEYCODE_P:
            findPrev();
            break;
        case KEYCODE_N:
            findNext();
            break;
        case KEYCODE_R:
            replaceNext();
            break;
        case KEYCODE_A:
            replaceAll();
            break;
        case KEYCODE_Q:
            findModeOff();
            break;
        default:
            onTextControlModifiedKeyPressed(editor, event);
    }
}

void View::onTitleFieldModifiedKeyPressed(bb::cascades::KeyEvent *event)
{
    switch (event->keycap()) {
        case KEYCODE_S:
            onSaveTriggered();
            break;
        // TODO: add shortcut for open and new file, next/prev tab
        default:
            onTextControlModifiedKeyPressed(_titleField->editor(), event);
    }
}

// this method should only be invoked in normal mode
void View::onTextControlModifiedKeyPressed(bb::cascades::TextEditor *editor, bb::cascades::KeyEvent *event)
{
    switch (event->keycap()) {
        case KEYCODE_V: {
            bb::system::Clipboard clipboard;
            QString paste = clipboard.value("text/plain");
            editor->insertPlainText(paste);
            break;
        }
    }
}

void View::onTextAreaModKeyPressed(bb::cascades::KeyEvent *event)
{
    _textArea->editor()->insertPlainText(event->unicode());
}

void View::onTextAreaModifiedKeyPressed(bb::cascades::KeyEvent *event)
{
    switch (event->keycap()) {
        case KEYCODE_T:
            _textArea->editor()->insertPlainText("\t");
            break;
        case KEYCODE_Z:
            onUndoTriggered();
            break;
        // TODO: subject to change in keycode
        case KEYCODE_G:
            _titleField->requestFocus();
            break;
        case KEYCODE_X:
            onRedoTriggered();
            break;
        case KEYCODE_F:
            findModeOn();
            break;
        case KEYCODE_S:
            onSaveTriggered();
            break;
        case KEYCODE_D:
            killCurrentLine();
            break;
        // TODO: add shortcut for open and new file, next/prev tab
        default:
            onTextControlModifiedKeyPressed(_textArea->editor(), event);
    }
}

void View::killCurrentLine() {
    _buffer->killLine(this, _textArea->editor()->cursorPosition());
}

void View::onTextAreaTextChanged(const QString& text)
{
    if (_modifyingTextArea) {
        return;
    }
    qDebug() << "## parsing change from text area";
//    qDebug() << "lines from the highlightStart:" <<
//            text.right(text.size() - _highlightStart.htmlCount + 1).left(100);
    _buffer->parseChange(this, text, _highlightStart, _textArea->editor()->cursorPosition());
}

Range View::partialHighlightRange(const BufferState &st, Range focus)
{
    focus.grow(PARTIAL_HIGHLIGHT_RANGE).clamp(0, st.size());
    // extend the range to make sure that we have non-empty lines at two ends
    while (focus.from > 0 && st[focus.from].line.isEmpty()) {
        focus.from--;
    }
    while (focus.to > 0 && focus.to < st.size() && st[focus.to-1].line.isEmpty()) {
        focus.to++;
    }
    return focus;
}

void View::updateTextAreaPartialHighlight()
{
    _modifyingTextArea = true;
    int start = _textArea->editor()->selectionStart();
    int end = _textArea->editor()->selectionEnd();
    int startLine = _buffer->state().focus(start).lineIndex;
    int endLine = start == end ? startLine : _buffer->state().focus(end).lineIndex;
    // this is the case where we've just moved cursor to another place
    Range newRange = partialHighlightRange(_buffer->state(), Range(startLine, endLine));
    if (newRange != _highlightRange) {
        qDebug() << "## updating new partial highlight view from" << _highlightRange << "to" << newRange;
        _highlightRange = newRange;
        QString highlightedHtml;
        QTextStream output(&highlightedHtml);
        _highlightStart = _buffer->state().writeHighlightedHtml(output, _highlightRange);
        output.flush();
        qDebug() << "updated highlightStart to" << _highlightStart;
//        qDebug() << "lines from the highlightStart:" <<
//                highlightedHtml.right(highlightedHtml.size() - _highlightStart.htmlCount + 1).left(100);
//        qDebug() << "entire doc:" << highlightedHtml;
        _textArea->setText(highlightedHtml);
        _textArea->editor()->setSelection(start, end);
    }
    _modifyingTextArea = false;
}

void View::onTextAreaCursorPositionChanged(int cursorPosition)
{
    if (_modifyingTextArea || _buffer->filetype().isEmpty()) {
        return;
    }
    if (!_partialHighlightUpdateTimer.isActive())
        _partialHighlightUpdateTimer.start();
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

// Is textChanging or cursorPositionChanged emitted first?
void View::onBufferStateChanged(BufferState& state, View *source, bool sourceChanged) {
    if (this != source || sourceChanged) {
        int pos = this == source ? state.cursorPosition() : _textArea->editor()->cursorPosition();
        // we assume that selectionStart == selectionEnd == pos in this case
        _modifyingTextArea = true;
        _highlightRange = partialHighlightRange(state, Range(state.focus(pos).lineIndex));
        QString highlightedHtml;
        QTextStream output(&highlightedHtml);
        _highlightStart = state.writeHighlightedHtml(output, _highlightRange);
        output.flush();

        qDebug() << "## text area out of sync";
        qDebug() << "### text area:";
        qDebug() << _textArea->text();
        qDebug() << "### buffer:";
        qDebug() << highlightedHtml;

        _textArea->setText(highlightedHtml);
        _textArea->editor()->setCursorPosition(pos);
        _modifyingTextArea = false;
    }
}

void View::onBufferProgressChanged(float progress)
{
    _progressIndicator->setValue(progress);
    _progressIndicator->setVisible(progress > 0 && progress < 1);
}

void View::onBufferLockedChanged(bool locked)
{
    switch (_mode) {
        case Normal:
            _textArea->setEditable(!locked);
            _titleField->setEnabled(!locked);
            // actions
            _saveAction->setEnabled(!locked);
            _undoAction->setEnabled(!locked && _buffer->hasUndo());
            _redoAction->setEnabled(!locked && _buffer->hasRedo());
            _findAction->setEnabled(!locked);
            break;
        case Find:
            _findField->setEnabled(!locked);
            _replaceField->setEnabled(!locked);
            // actions
            _goToFindFieldAction->setEnabled(!locked);
            _findPrevAction->setEnabled(!locked);
            _findNextAction->setEnabled(!locked);
            _replaceNextAction->setEnabled(!locked);
            _replaceAllAction->setEnabled(!locked);
            _findCancelAction->setEnabled(!locked);
            _undoAction->setEnabled(!locked && _buffer->hasUndo());
            _redoAction->setEnabled(!locked && _buffer->hasRedo());
            break;
    }
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
    _saveAction->setTitle(tr(ACTION_TITLE_SAVE));
    _undoAction->setTitle(tr(ACTION_TITLE_UNDO));
    _redoAction->setTitle(tr(ACTION_TITLE_REDO));
    _findAction->setTitle(tr(ACTION_TITLE_FIND));
}

void View::reloadFindModeActionTitles()
{
    _goToFindFieldAction->setTitle(tr(ACTION_TITLE_GO_TO_FIND_FIELD));
    _findPrevAction->setTitle(tr(ACTION_TITLE_FIND_PREV));
    _findNextAction->setTitle(tr(ACTION_TITLE_FIND_NEXT));
    _replaceNextAction->setTitle(tr(ACTION_TITLE_REPLACE_NEXT));
    _replaceAllAction->setTitle(tr(ACTION_TITLE_REPLACE_ALL));
    _findCancelAction->setTitle(tr(ACTION_TITLE_FIND_CANCEL));
    _undoAction->setTitle(tr(ACTION_TITLE_UNDO));
    _redoAction->setTitle(tr(ACTION_TITLE_REDO));
}

void View::onLanguageChanged()
{
    _titleField->setHintText(tr(HINT_TEXT_TITLE_FIELD));
    _textArea->setHintText(tr(HINT_TEXT_TEXT_AREA));

    _findCaseSensitiveCheckBox->setText(tr(CHECKBOX_TEXT_FIND_CASE_SENSITIVE));
    _findField->setHintText(tr(HINT_TEXT_FIND_FIELD));
    _replaceField->setHintText(tr(HINT_TEXT_REPLACE_FIELD));

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
