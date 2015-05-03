/*
 * View.cpp
 *
 *  Created on: Nov 21, 2014
 *      Author: lingnan
 */

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
#include <bb/cascades/pickers/FilePicker>
#include <bb/system/Clipboard>
#include <View.h>
#include <MultiViewPane.h>
#include <Buffer.h>
#include <ModKeyListener.h>
#include <SignalBlocker.h>
#include <Utility.h>

using namespace bb::cascades;

// keys
#define KEYFLAG_NONE 0
#define KEYFLAG_RETURN 1
// the one side range for partial highlight (will be put into settings)
#define PARTIAL_HIGHLIGHT_RANGE 20

// #### View
// TODO: it would be good if you can modulize the undo/redo functionality
// and then attach it to each textfield/area like what you did with the modkeylistener!
// that would be awesome...
View::View(Buffer *buffer):
        _buffer(NULL),
        _findTitleBar(NULL),
        _fpicker(NULL),
        _mode(Normal),
        _findBufferDirty(true)
{
    _textArea = TextArea::create()
        .format(TextFormat::Html)
        .inputFlags(TextInputFlag::SpellCheckOff
                    | TextInputFlag::AutoCorrectionOff
                    | TextInputFlag::AutoCapitalizationOff
                    | TextInputFlag::AutoPeriodOff)
        .contentFlags(TextContentFlag::ActiveTextOff)
        .focusPolicy(FocusPolicy::Touch)
        .layoutProperties(StackLayoutProperties::create()
            .spaceQuota(1))
        .bottomMargin(0);
    _textAreaModKeyListener = ModKeyListener::create(KEYCODE_RETURN)
        .onModifiedKeyPressed(this, SLOT(onTextAreaModifiedKeyPressed(bb::cascades::KeyEvent*, ModKeyListener*)))
        .onModKeyPressed(this, SLOT(onTextAreaModKeyPressed(bb::cascades::KeyEvent*)))
        .onTextAreaInputModeChanged(_textArea, SLOT(setInputMode(bb::cascades::TextAreaInputMode::Type)))
        .modOffOn(_textArea, SIGNAL(focusedChanged(bool)));
    _textArea->addKeyListener(_textAreaModKeyListener);
    conn(_textArea->editor(), SIGNAL(cursorPositionChanged(int)),
            this, SLOT(onTextAreaCursorPositionChanged()));
    // TODO: monospace font doesn't play well with italic
    // so what we need to do is to reformat the style file for highlight
//    _textArea->textStyle()->setFontFamily("\"Andale Mono\", monospace");

    // setup the timer for partial highlight update
    _partialHighlightUpdateTimer.setSingleShot(true);
    conn(&_partialHighlightUpdateTimer, SIGNAL(timeout()),
            this, SLOT(updateTextAreaPartialHighlight()));


    // title bar
    TextFieldTitleBarKindProperties *titleBarProperties = new TextFieldTitleBarKindProperties;
    _titleField = titleBarProperties->textField();
    _titleField->setFocusPolicy(FocusPolicy::Touch);
    _titleField->setBackgroundVisible(true);
    _titleField->textStyle()->setBase(SystemDefaults::TextStyles::titleText());
    conn(_titleField, SIGNAL(focusedChanged(bool)),
        this, SLOT(onTitleFieldFocusChanged(bool)));
    _titleField->addKeyListener(ModKeyListener::create(KEYCODE_RETURN)
        .onModifiedKeyPressed(this, SLOT(onTitleFieldModifiedKeyPressed(bb::cascades::KeyEvent*, ModKeyListener*)))
        .onModKeyPressed(_textArea, SLOT(requestFocus()))
        .onTextFieldInputModeChanged(_titleField, SLOT(setInputMode(bb::cascades::TextFieldInputMode::Type)))
        .modOffOn(_titleField, SIGNAL(focusedChanged(bool))));
    // the default titlebar
    _titleBar = TitleBar::create(TitleBarKind::TextField)
        .kindProperties(titleBarProperties);

    _progressIndicator = ProgressIndicator::create()
        .visible(false)
        .vertical(VerticalAlignment::Bottom)
        .topMargin(0);

    // the page
    setContent(Page::create()
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
            .onTriggered(this, SLOT(autoFocus()))));

    // actions and text
    setNormalModeActions();
    onTranslatorChanged();

    setBuffer(buffer);
}

Page *View::content() const
{
    return (Page *) bb::cascades::Tab::content();
}

MultiViewPane *View::parent() const
{
    return (MultiViewPane *) QObject::parent();
}

Buffer *View::buffer() const
{
    return _buffer;
}

/* miscellaneous actions */
void View::onOutOfView()
{
    findModeOff();
    // default to lose focus for the text area
    _textArea->loseFocus();
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

void View::setHistoryActions()
{
    _undoAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_undo.png"))
        .addShortcut(Shortcut::create().key("z"))
        .onTriggered(this, SLOT(onUndoTriggered()));
    conn(this, SIGNAL(hasUndosChanged(bool)), _undoAction, SLOT(setEnabled(bool)));
    _redoAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_redo.png"))
        .addShortcut(Shortcut::create().key("r"))
        .onTriggered(this, SLOT(onRedoTriggered()));
    conn(this, SIGNAL(hasRedosChanged(bool)), _redoAction, SLOT(setEnabled(bool)));
}

void View::setNormalModeActions()
{
    content()->removeAllActions();
    _saveAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_save.png"))
        .addShortcut(Shortcut::create().key("s"))
        .onTriggered(this, SLOT(save()));
    _saveAsAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_save_as.png"))
        .onTriggered(this, SLOT(saveAs()));
    _openAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_open.png"))
        .addShortcut(Shortcut::create().key("e"))
        .onTriggered(this, SLOT(open()));
    setHistoryActions();
    _findAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_search.png"))
        .addShortcut(Shortcut::create().key("f"))
        .onTriggered(this, SLOT(findModeOn()));
    _cloneAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_copy_link.png"))
        .addShortcut(Shortcut::create().key("y"))
        .onTriggered(this, SLOT(clone()));
    _closeAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_clear.png"))
        .addShortcut(Shortcut::create().key("x"))
        .onTriggered(this, SLOT(close()));
    reloadNormalModeActionTitles();
    content()->addAction(_saveAction, ActionBarPlacement::Signature);
    content()->addAction(_undoAction, ActionBarPlacement::OnBar);
    content()->addAction(_redoAction, ActionBarPlacement::OnBar);
    content()->addAction(_saveAsAction);
    content()->addAction(_openAction);
    content()->addAction(_findAction);
    content()->addAction(_cloneAction);
    content()->addAction(_closeAction);
}

void View::setFindModeActions()
{
    content()->removeAllActions();
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
    setHistoryActions();
    reloadFindModeActionTitles();
    content()->addAction(_goToFindFieldAction);
    content()->addAction(_findPrevAction);
    content()->addAction(_findNextAction, ActionBarPlacement::Signature);
    content()->addAction(_replaceNextAction, ActionBarPlacement::OnBar);
    content()->addAction(_replaceAllAction);
    content()->addAction(_findCancelAction, ActionBarPlacement::OnBar);
    content()->addAction(_undoAction);
    content()->addAction(_redoAction);
}

bool View::findModeOn()
{
    if (_mode != Find) {
        _mode = Find;
        // disable current key handler
        _textAreaModKeyListener->setEnabled(false);
        // titlebar
        if (!_findTitleBar) {
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
                .modOffOn(_findField, SIGNAL(focusedChanged(bool))));
            _replaceField->addKeyListener(ModKeyListener::create(KEYCODE_RETURN)
                .onModifiedKeyPressed(this, SLOT(onReplaceFieldModifiedKeyPressed(bb::cascades::KeyEvent*)))
                .onModKeyPressed(this, SLOT(findNext()))
                .onTextFieldInputModeChanged(_replaceField, SLOT(setInputMode(bb::cascades::TextFieldInputMode::Type)))
                .modOffOn(_replaceField, SIGNAL(focusedChanged(bool))));
            _findCaseSensitiveCheckBox = new CheckBox;
            _findCancelButton = Button::create().vertical(VerticalAlignment::Center)
                .preferredWidth(0)
                .onClicked(this, SLOT(findModeOff()));
            FreeFormTitleBarKindProperties *findTitleBarProperties = FreeFormTitleBarKindProperties::create()
                .expandableIndicator(TitleBarExpandableAreaIndicatorVisibility::Hidden)
                .content(Container::create()
                    .layout(StackLayout::create().orientation(LayoutOrientation::LeftToRight))
                    .left(20).right(20)
                    .add(Button::create().vertical(VerticalAlignment::Center)
                        .imageSource((QUrl("asset:///images/ic_view_details.png")))
                        .preferredWidth(0)
                        .onClicked(this, SLOT(onFindOptionButtonClicked())))
                    .add(_findField)
                    .add(_replaceField)
                    .add(_findCancelButton))
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
            reloadFindTitleBarLabels();
        }
        content()->setTitleBar(_findTitleBar);
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

        content()->setTitleBar(_titleBar);
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
            Utility::toast(tr("Search query can't be empty!"));
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
        _findBuffer = _buffer->state().plainText().toStdWString();
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
                    Utility::toast(tr("Not found"));
                return;
            }
            // check if there is something on findIndex
            ++_findIndex;
            if (_findComplete && _findIndex == _findHits.size()) {
                _findIndex = 0;
            }
            if (_findIndex == _bofIndex && interactive) {
                Utility::toast(tr("Reached end of document, beginning from top!"));
            }
            if (_findIndex < _findHits.size()) {
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
                    Utility::toast(tr("Not found"));
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
                    Utility::toast(tr("Not found"));
                Q_ASSERT(_findHits.isEmpty());
                _findComplete = true;
                return;
            }
            _bofIndex = _findIndex;
            if (interactive)
                Utility::toast(tr("Reached end of document, beginning from top!"));
        }
    }
    // found something
    // decide whether we need to insert the location into the list
    qDebug() << "current findIndex:" << _findIndex << "findhits size:" << _findHits.size();
    Q_ASSERT(_findIndex == _findHits.size());
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
                        Utility::toast(tr("Reached top of document, beginning from bottom!"));
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
                Utility::toast(tr("Not found"));
            } else {
                select(_findHits[_findIndex].selection);
            }
            break;
        }
        case Unchanged: {
            _textArea->requestFocus();
            if (_findComplete && _findHits.isEmpty()) {
                Utility::toast(tr("Not found"));
                return;
            }
            if (_findIndex == _bofIndex) {
                Utility::toast(tr("Reached top of document, beginning from bottom!"));
            }
            // check if there is something on findIndex
            --_findIndex;
            if (_findComplete && _findIndex < 0) {
                _findIndex = _findHits.size() - 1;
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
                            Utility::toast(tr("Not found"));
                            _findComplete = true;
                            break;
                        }
                        _bofIndex = _findHits.size();
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
                    if (_bofIndex == _findHits.size()) {
                        _bofIndex = 0;
                        Utility::toast(tr("Reached top of document, beginning from bottom!"));
                    }
                    break;
                } else {
                    _findHits.append(FindMatch(selection, *_findIterator));
                }
            }
            _findIndex = _findHits.size() - 1;
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
        if (_findIndex >= 0 && _findIndex < _findHits.size() && current == _findHits[_findIndex].selection) {
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
    for (; i < _findHits.size(); i++) {
        index = (replaceIndex + i) % _findHits.size();
//        qDebug() << "adding index" << index << "to the first replaces queue";
        if (index == _bofIndex) {
            break;
        }
        _replaces.append(Replacement(_findHits[index].selection,
                QString::fromStdWString(_findHits[index].match.format(replacement))));
    }
    // do the actual replaces
    _buffer->parseReplacement(_replaces);
    _numberOfReplacesTillBottom = _replaces.size();
    _replaces.clear();
    for (; i < _findHits.size(); i++) {
        index = (replaceIndex + i) % _findHits.size();
//        qDebug() << "adding index" << index << "to the second replaces queue";
        _replaces.append(Replacement(_findHits[index].selection,
                QString::fromStdWString(_findHits[index].match.format(replacement))));
    }
    // mark the buffer as dirty
    _findBufferDirty = true;
    Utility::dialog(tr("Yes"), tr("No"), tr("End of file reached"),
            tr("%n occurrence(s) replaced. Do you want to continue from the beginning?", "", _numberOfReplacesTillBottom),
            this, SLOT(onReplaceFromTopDialogFinished(bb::system::SystemUiResult::Type)));
}

void View::onReplaceFromTopDialogFinished(bb::system::SystemUiResult::Type type)
{
    if (type == bb::system::SystemUiResult::ConfirmButtonSelection) {
        _buffer->parseReplacement(_replaces);
        Utility::dialog(tr("OK"), tr("Replace finished"),
                tr("%n occurrence(s) replaced.", "", _numberOfReplacesTillBottom+_replaces.size()));
    }
    _replaces.clear();
}

void View::setTitle(const QString &title)
{
    if (title.isEmpty()) {
        Tab::setTitle(tr("No Name"));
    } else {
        Tab::setTitle(title);
    }
    if (!_titleField->isEnabled()) {
        // TODO: hack to get around the problem of title field not grey
        _titleField->setEnabled(true);
        _titleField->setText(title);
        _titleField->setEnabled(false);
    } else {
        _titleField->setText(title);
    }
}

void View::setBuffer(Buffer *buffer)
{
    if (buffer != _buffer) {
        findModeOff();
        if (_buffer) {
            _buffer->detachView(this);
            disconn(_textArea, SIGNAL(textChanging(const QString)),
                this, SLOT(onTextAreaTextChanged(const QString&)));
            disconn(_buffer, SIGNAL(stateChanged(const StateChangeContext&, const BufferState&)),
                this, SLOT(onBufferStateChanged(const StateChangeContext&, const BufferState&)));
            disconn(_buffer, SIGNAL(nameChanged(const QString&)),
                this, SLOT(setTitle(const QString&)));
            disconn(_buffer, SIGNAL(filetypeChanged(const QString&)),
                this, SLOT(onBufferFiletypeChanged(const QString&)));
            disconn(_buffer, SIGNAL(filepathChanged(const QString&)),
                this, SLOT(onBufferFilepathChanged(const QString&)));
            disconn(_buffer, SIGNAL(dirtyChanged(bool)),
                this, SLOT(onBufferDirtyChanged(bool)));
            disconn(_buffer, SIGNAL(progressChanged(float, bb::cascades::ProgressIndicatorState::Type, const QString&)),
                this, SLOT(onBufferProgressChanged(float, bb::cascades::ProgressIndicatorState::Type, const QString&)));
            disconn(_buffer, SIGNAL(lockedChanged(bool)),
                this, SLOT(onBufferLockedChanged(bool)));
            disconn(_buffer, SIGNAL(hasUndosChanged(bool)),
                this, SIGNAL(hasUndosChanged(bool)));
            disconn(_buffer, SIGNAL(hasRedosChanged(bool)),
                this, SIGNAL(hasRedosChanged(bool)));
            disconn(_buffer, SIGNAL(savedToFile(const QString&)),
                this, SLOT(onBufferSavedToFile(const QString&)))
            // remove the buffer if it no longer has other views attached
            if (_buffer->views().empty())
                parent()->removeBuffer(_buffer);
        }
        _buffer = buffer;
        if (_buffer) {
            _buffer->attachView(this);
            onBufferStateChanged(StateChangeContext(), _buffer->state());
            conn(_textArea, SIGNAL(textChanging(const QString)),
                this, SLOT(onTextAreaTextChanged(const QString&)));
            conn(_buffer, SIGNAL(stateChanged(const StateChangeContext&, const BufferState&)),
                this, SLOT(onBufferStateChanged(const StateChangeContext&, const BufferState&)));

            setTitle(_buffer->name());
            conn(_buffer, SIGNAL(nameChanged(const QString&)),
                this, SLOT(setTitle(const QString&)));

            onBufferFiletypeChanged(_buffer->filetype());
            conn(_buffer, SIGNAL(filetypeChanged(const QString&)),
                this, SLOT(onBufferFiletypeChanged(const QString&)));

            onBufferFilepathChanged(_buffer->filepath());
            conn(_buffer, SIGNAL(filepathChanged(const QString&)),
                this, SLOT(onBufferFilepathChanged(const QString&)));

            onBufferDirtyChanged(_buffer->dirty());
            conn(_buffer, SIGNAL(dirtyChanged(bool)),
                this, SLOT(onBufferDirtyChanged(bool)));

            conn(_buffer, SIGNAL(progressChanged(float, bb::cascades::ProgressIndicatorState::Type, const QString&)),
                this, SLOT(onBufferProgressChanged(float, bb::cascades::ProgressIndicatorState::Type, const QString&)));

            onBufferLockedChanged(_buffer->locked());
            conn(_buffer, SIGNAL(lockedChanged(bool)),
                this, SLOT(onBufferLockedChanged(bool)));

            _undoAction->setEnabled(_buffer->hasUndo());
            conn(_buffer, SIGNAL(hasUndosChanged(bool)),
                this, SIGNAL(hasUndosChanged(bool)));

            _redoAction->setEnabled(_buffer->hasRedo());
            conn(_buffer, SIGNAL(hasRedosChanged(bool)),
                this, SIGNAL(hasRedosChanged(bool)));

            conn(_buffer, SIGNAL(savedToFile(const QString&)),
                this, SLOT(onBufferSavedToFile(const QString&)));
        }
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

void View::onTitleFieldModifiedKeyPressed(bb::cascades::KeyEvent *event, ModKeyListener *listener)
{
    switch (event->keycap()) {
        case KEYCODE_S:
            if (save() == OpenedFilePicker)
                listener->modOff();
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

void View::onTextAreaModifiedKeyPressed(bb::cascades::KeyEvent *event, ModKeyListener *listener)
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
        case KEYCODE_R:
            onRedoTriggered();
            break;
        case KEYCODE_F:
            findModeOn();
            break;
        case KEYCODE_S:
            if (save() == OpenedFilePicker)
                listener->modOff();
            break;
        case KEYCODE_E:
            open(); // this always opens some sort of UI
            listener->modOff();
            break;
        case KEYCODE_D:
            killCurrentLine();
            break;
        case KEYCODE_X:
            close();
            break;
        // TODO: subject to change in keycode
        case KEYCODE_Y:
            clone();
            break;
        case KEYCODE_C:
            parent()->addNewView();
            break;
        case KEYCODE_Q:
            parent()->setPrevViewActive();
            break;
        case KEYCODE_W:
            parent()->setNextViewActive();
            break;
        default:
            onTextControlModifiedKeyPressed(_textArea->editor(), event);
    }
}

void View::killCurrentLine() {
    _buffer->killLine(this, _textArea->editor()->cursorPosition());
}

void View::onTextAreaTextChanged(const QString& text)
{
    qDebug() << "## parsing change from text area";
//    qDebug() << "lines from the highlightStart:" <<
//            text.right(text.size() - _highlightStart.htmlCount + 1).left(100);
    _buffer->parseChange(this, text, _highlightStart, _textArea->editor()->cursorPosition());
}

Range View::partialHighlightRange(const BufferState &st, Range focus)
{
    // NOTE: the minium range would be (0,1)
    focus.grow(PARTIAL_HIGHLIGHT_RANGE).clamp(0, qMax(st.size(), 1));
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
    SignalBlocker blocker(_textArea);
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
}

void View::onTextAreaCursorPositionChanged()
{
    if (_buffer->filetype().isEmpty()) {
        return;
    }
    if (!_partialHighlightUpdateTimer.isActive())
        _partialHighlightUpdateTimer.start();
}

void View::onBufferFiletypeChanged(const QString &filetype)
{
    if (filetype.isEmpty()) {
        // use a default image
        // TODO: fill in the images
//        setImageSource(QUrl("asset:///images/txt.png"));
    } else {
        Utility::toast(tr("Filetype set to %1").arg(filetype));
//        setImageSource(QUrl("asset:///images/"+filetype+".png"));
    }
}

void View::onBufferFilepathChanged(const QString &filepath)
{
    qDebug() << "filepath set to" << filepath;
    _titleField->setEnabled(filepath.isEmpty());
    // TODO: we might also want to change the font color to grey
    // to make it clearer to the user
}

// Is textChanging or cursorPositionChanged emitted first?
void View::onBufferStateChanged(const StateChangeContext &ctx, const BufferState &state)
{
    if (this != ctx.sourceView || ctx.sourceViewShouldUpdate) {
        int pos = ctx.shouldMatchCursorPosition || this == ctx.sourceView ?
                state.cursorPosition() :
                _textArea->editor()->cursorPosition();
        // we assume that selectionStart == selectionEnd == pos in this case
        SignalBlocker blocker(_textArea);
        _highlightRange = partialHighlightRange(state, Range(state.focus(pos).lineIndex));
        QString highlightedHtml;
        QTextStream output(&highlightedHtml);
        _highlightStart = state.writeHighlightedHtml(output, _highlightRange);
        output.flush();

        qDebug() << "### text area:";
        qDebug() << _textArea->text();
        qDebug() << "### buffer:";
        qDebug() << highlightedHtml;

        _textArea->setText(highlightedHtml);
        _textArea->editor()->setCursorPosition(pos);
    }
}

void View::onBufferProgressChanged(float progress, bb::cascades::ProgressIndicatorState::Type state, const QString &msg)
{
    qDebug() << "received progress change" << progress;
    _progressIndicator->setState(state);
    _progressIndicator->setValue(progress);
    _progressIndicator->setVisible(progress > 0 && progress < 1);
    if (!msg.isNull())
        Utility::toast(msg, tr("OK"), this, SLOT(onProgressMessageDismissed(bb::system::SystemUiResult::Type)));
}

void View::onProgressMessageDismissed(bb::system::SystemUiResult::Type)
{
    _progressIndicator->setVisible(false);
}

void View::onBufferLockedChanged(bool locked)
{
    switch (_mode) {
        case Normal:
            _textArea->setEditable(!locked);
            _titleField->setEnabled(!locked && _buffer->filepath().isEmpty());
            // actions
            _saveAction->setEnabled(!locked);
            _saveAsAction->setEnabled(!locked);
            _openAction->setEnabled(!locked);
            _undoAction->setEnabled(!locked && _buffer->hasUndo());
            _redoAction->setEnabled(!locked && _buffer->hasRedo());
            _findAction->setEnabled(!locked);
            _cloneAction->setEnabled(!locked);
            _closeAction->setEnabled(!locked);
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

void View::onBufferDirtyChanged(bool dirty)
{
    _titleField->textStyle()->setFontStyle(dirty ? FontStyle::Italic : FontStyle::Normal);
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
    _saveAsAction->setTitle(tr("Save As"));
    _openAction->setTitle(tr("Open"));
    _undoAction->setTitle(tr("Undo"));
    _redoAction->setTitle(tr("Redo"));
    _findAction->setTitle(tr("Find"));
    _cloneAction->setTitle(tr("Clone"));
    _closeAction->setTitle(tr("Close"));
}

void View::reloadFindModeActionTitles()
{
    _goToFindFieldAction->setTitle(tr("Go to find field"));
    _findPrevAction->setTitle(tr("Find previous"));
    _findNextAction->setTitle(tr("Find next"));
    _replaceNextAction->setTitle(tr("Replace next"));
    _replaceAllAction->setTitle(tr("Replace all remaining"));
    _findCancelAction->setTitle(tr("Cancel"));
    _undoAction->setTitle(tr("Undo"));
    _redoAction->setTitle(tr("Redo"));
}

void View::reloadFindTitleBarLabels()
{
    _findCaseSensitiveCheckBox->setText(tr("Case sensitive"));
    _findField->setHintText(tr("Find text"));
    _replaceField->setHintText(tr("Replace with"));
    _findCancelButton->setText(tr("Cancel"));
}

void View::onTranslatorChanged()
{
    _titleField->setHintText(tr("Enter the title"));
    _textArea->setHintText(tr("Enter the content"));

    if (_findTitleBar) {
        reloadFindTitleBarLabels();
    }

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

/* file related operations */
bb::cascades::pickers::FilePicker *View::filePicker()
{
    if (!_fpicker) {
        _fpicker = new bb::cascades::pickers::FilePicker(this);
        _fpicker->setType(bb::cascades::pickers::FileType::Document);
        _fpicker->setFilter(QStringList("*"));
        conn(_fpicker, SIGNAL(fileSelected(const QStringList&)),
                this, SLOT(onFileSelected(const QStringList&)));
    }
    if (_buffer->filepath().isEmpty()) {
        _fpicker->setDirectories(QStringList("/accounts/1000/shared"));
    } else {
        _fpicker->setDirectories(QStringList(QFileInfo(_buffer->filepath()).absolutePath()));
    }
    return _fpicker;
}

View::SaveStatus View::save()
{
    if (!_buffer->filepath().isEmpty()) {
        _buffer->save(_buffer->filepath());
        return Finished;
    }
    return saveAs();
}

View::SaveStatus View::saveAs()
{
    filePicker()->setMode(bb::cascades::pickers::FilePickerMode::Saver);
    filePicker()->setDefaultSaveFileNames(QStringList(_buffer->name()));
    filePicker()->setTitle(tr("Save"));
    filePicker()->open();
    return OpenedFilePicker;
}

void View::onBufferSavedToFile(const QString &)
{
    Utility::toast(tr("Saved"));
}

void View::open()
{
    if  (_buffer->views().size() == 1 && _buffer->dirty()) {
        Utility::dialog(tr("Yes"), tr("No"), tr("Unsaved change detected"),
                tr("Do you want to continue?"),
                this, SLOT(onUnsavedChangeDialogFinishedWhenOpening(bb::system::SystemUiResult::Type)));
    } else {
        pickFileToOpen();
    }
}

void View::onUnsavedChangeDialogFinishedWhenOpening(bb::system::SystemUiResult::Type type)
{
    if (type == bb::system::SystemUiResult::ConfirmButtonSelection) {
        pickFileToOpen();
    }
}

void View::pickFileToOpen()
{
    filePicker()->setMode(bb::cascades::pickers::FilePickerMode::Picker);
    filePicker()->setTitle(tr("Open"));
    filePicker()->open();
}

void View::onFileSelected(const QStringList &files)
{
    switch (filePicker()->mode()) {
        case bb::cascades::pickers::FilePickerMode::Picker:
            if (files[0] != _buffer->filepath()) {
                Buffer *b =parent()->bufferForFilepath(files[0]);
                if (b) {
                    setBuffer(b);
                } else {
                    if (_buffer->views().size() != 1) {
                        // not only bound to this view!
                        setBuffer(parent()->newBuffer());
                    }
                    _buffer->load(files[0]);
                }
            }
            break;
        case bb::cascades::pickers::FilePickerMode::Saver:
            _buffer->save(files[0]);
            break;
    }
}

void View::clone()
{
    parent()->cloneActive();
}

void View::close()
{
    if  (_buffer->views().size() == 1 && _buffer->dirty()) {
        Utility::dialog(tr("Yes"), tr("No"), tr("Unsaved change detected"),
                tr("Do you want to continue?"),
                this, SLOT(onUnsavedChangeDialogFinishedWhenClosing(bb::system::SystemUiResult::Type)));
    } else {
        parent()->remove(this);
    }
}

void View::onUnsavedChangeDialogFinishedWhenClosing(bb::system::SystemUiResult::Type type)
{
    if (type == bb::system::SystemUiResult::ConfirmButtonSelection) {
        parent()->remove(this);
    }
}
