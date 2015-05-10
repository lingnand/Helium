/*
 * FindMode.cpp
 *
 *  Created on: May 4, 2015
 *      Author: lingnan
 */

#include <bb/cascades/TextField>
#include <bb/cascades/TextArea>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/TitleBar>
#include <bb/cascades/FreeFormTitleBarKindProperties>
#include <bb/cascades/Button>
#include <bb/cascades/Container>
#include <bb/cascades/ActionItem>
#include <bb/cascades/Shortcut>
#include <bb/cascades/CheckBox>
#include <bb/cascades/UIPalette>
#include <bb/cascades/Page>
#include <bb/cascades/KeyEvent>
#include <bb/cascades/Tab>
#include <FindMode.h>
#include <View.h>
#include <MultiViewPane.h>
#include <Buffer.h>
#include <ModKeyListener.h>
#include <SignalBlocker.h>
#include <Utility.h>

using namespace bb::cascades;

FindMode::FindMode(View *view):
    ViewMode(view),
    _findField(TextField::create().vertical(VerticalAlignment::Center)
        .focusPolicy(FocusPolicy::Touch)
        .layoutProperties(StackLayoutProperties::create().spaceQuota(1))),
    _replaceField(TextField::create().vertical(VerticalAlignment::Center)
        .focusPolicy(FocusPolicy::Touch)
        .layoutProperties(StackLayoutProperties::create().spaceQuota(1))),
    _goToFindFieldAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_search.png"))
        .addShortcut(Shortcut::create().key("f"))
        .onTriggered(_findField, SLOT(requestFocus()))),
    _findPrevAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_backward.png"))
        .addShortcut(Shortcut::create().key("p"))
        .onTriggered(this, SLOT(findPrev()))),
    _findNextAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_forward.png"))
        .addShortcut(Shortcut::create().key("n"))
        .onTriggered(this, SLOT(findNext()))),
    _replaceNextAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_rename.png"))
        .addShortcut(Shortcut::create().key("r"))
        .onTriggered(this, SLOT(replaceNext()))),
    _replaceAllAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_rename.png"))
        .addShortcut(Shortcut::create().key("a"))
        .onTriggered(this, SLOT(replaceAll()))),
    _undoAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_undo.png"))
        .addShortcut(Shortcut::create().key("z"))
        .onTriggered(view, SIGNAL(undo()))),
    _redoAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_redo.png"))
        .addShortcut(Shortcut::create().key("y"))
        .onTriggered(view, SIGNAL(redo()))),
    _findCancelAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_cancel.png"))
        .addShortcut(Shortcut::create().key("x"))
        .onTriggered(view, SLOT(setNormalMode()))),
    _regexOption(Tab::create()
        .onTriggered(this, SLOT(onRegexSelected()))),
    _ignoreCaseOption(Tab::create()
        .onTriggered(this, SLOT(onIgnoreCaseSelected()))),
    _exactMatchOption(Tab::create()
        .onTriggered(this, SLOT(onExactMatchSelected()))),
    _findBufferDirty(true)
{
    // TODO: not only focus the replaceField, but also select the existing content of the replace field
    _findField->addKeyListener(ModKeyListener::create(KEYCODE_RETURN)
        .onModifiedKeyPressed(this, SLOT(onFindFieldModifiedKeyPressed(bb::cascades::KeyEvent*)))
        .onModKeyPressed(_replaceField, SLOT(requestFocus()))
        .onTextFieldInputModeChanged(_findField, SLOT(setInputMode(bb::cascades::TextFieldInputMode::Type)))
        .modOffOn(_findField, SIGNAL(focusedChanged(bool))));
    conn(_findField, SIGNAL(focusedChanged(bool)),
        view, SLOT(blockPageKeyListener(bool)));
    _replaceField->addKeyListener(ModKeyListener::create(KEYCODE_RETURN)
        .onModifiedKeyPressed(this, SLOT(onReplaceFieldModifiedKeyPressed(bb::cascades::KeyEvent*)))
        .onModKeyPressed(this, SLOT(findNext()))
        .onTextFieldInputModeChanged(_replaceField, SLOT(setInputMode(bb::cascades::TextFieldInputMode::Type)))
        .modOffOn(_replaceField, SIGNAL(focusedChanged(bool))));
    conn(_replaceField, SIGNAL(focusedChanged(bool)),
        view, SLOT(blockPageKeyListener(bool)));
    _findTitleBar = TitleBar::create(TitleBarKind::FreeForm)
        .kindProperties(FreeFormTitleBarKindProperties::create()
            .content(Container::create()
                .layout(StackLayout::create().orientation(LayoutOrientation::LeftToRight))
                .left(20).right(20)
                .add(_findField)
                .add(_replaceField)));

    conn(view, SIGNAL(hasUndosChanged(bool)), _undoAction, SLOT(setEnabled(bool)));
    conn(view, SIGNAL(hasRedosChanged(bool)), _redoAction, SLOT(setEnabled(bool)));

    onTranslatorChanged();
    conn(view, SIGNAL(translatorChanged()), this, SLOT(onTranslatorChanged()));
}

void FindMode::autoFocus(bool)
{
    _findField->requestFocus();
}

void FindMode::onEnter()
{
    view()->hideAllPageActions();

    setLocked(view()->buffer()->locked());
    conn(view(), SIGNAL(bufferLockedChanged(bool)), this, SLOT(setLocked(bool)));

    view()->page()->addAction(_goToFindFieldAction);
    view()->page()->addAction(_findPrevAction);
    view()->page()->addAction(_findNextAction, ActionBarPlacement::Signature);
    view()->page()->addAction(_replaceNextAction, ActionBarPlacement::OnBar);
    view()->page()->addAction(_replaceAllAction);
    view()->page()->addAction(_undoAction);
    view()->page()->addAction(_redoAction);
    view()->page()->addAction(_findCancelAction, ActionBarPlacement::OnBar);
    view()->page()->setTitleBar(_findTitleBar);

    // replace the tabs
    view()->parent()->hideViews();
    view()->detachPage();
    view()->parent()->setActivePane(view()->page());
    view()->parent()->add(_regexOption);
    view()->parent()->add(_ignoreCaseOption);
    view()->parent()->add(_exactMatchOption);

    view()->textArea()->setEditable(false);
    view()->textAreaModKeyListener()->setEnabled(false);

    _findField->requestFocus();
}

void FindMode::onExit()
{
    disconn(view(), SIGNAL(bufferLockedChanged(bool)), this, SLOT(setLocked(bool)));

    view()->parent()->setActivePane(NULL);
    view()->reattachPage();
    view()->parent()->restoreViews();

    _findBuffer.clear();
    _findQuery.clear();
    _findBufferDirty = true;
}

void FindMode::onRegexSelected()
{
    qDebug() << "regex selected";
}

void FindMode::onIgnoreCaseSelected()
{
    qDebug() << "ignore case selected";
}

void FindMode::onExactMatchSelected()
{
    qDebug() << "exact match selected";
}

void FindMode::onFindFieldModifiedKeyPressed(bb::cascades::KeyEvent *event)
{
    onFindFieldsModifiedKeyPressed(_findField->editor(), event);
}

void FindMode::onReplaceFieldModifiedKeyPressed(bb::cascades::KeyEvent *event)
{
    onFindFieldsModifiedKeyPressed(_replaceField->editor(), event);
}

void FindMode::onFindFieldsModifiedKeyPressed(bb::cascades::TextEditor *editor, bb::cascades::KeyEvent *event)
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
        case KEYCODE_X:
            view()->setNormalMode();
            break;
        default:
            view()->handleTextControlBasicModifiedKeys(editor, event);
    }
}

void FindMode::select(const TextSelection &selection)
{
    SignalBlocker blocker(view()->textArea());
    // use lose focus and then refocus to force scrolling to the right position
    view()->textArea()->loseFocus();
    view()->textArea()->editor()->setSelection(selection.start, selection.end);
    if (!view()->buffer()->filetype().isEmpty())
        view()->updateTextAreaPartialHighlight();
    view()->textArea()->requestFocus();
}

// this function is responsible for updating the find query/regex, if any change has taken place
FindMode::FindQueryUpdateStatus FindMode::updateFindQuery(bool interactive)
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
        _findBuffer = view()->buffer()->state().plainText().toStdWString();
        status = Changed;
    }
    if (status == Changed)
        _findComplete = false;
    return status;
}

void FindMode::findNext()
{
    findNextWithOptions(true, updateFindQuery(true));
}

void FindMode::findNextWithOptions(bool interactive, FindQueryUpdateStatus status)
{
    view()->textArea()->requestFocus();
    // check if we need to set up the iterator
    switch (status) {
        case Invalid:
            return;
        case Changed: {
            int findOffset = view()->textArea()->editor()->cursorPosition();
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

void FindMode::findPrev()
{
    view()->textArea()->requestFocus();
    switch (updateFindQuery(true)) {
        case Invalid:
            return;
        case Changed: {
            view()->textArea()->requestFocus();
            int findOffset = view()->textArea()->editor()->cursorPosition();
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
            view()->textArea()->requestFocus();
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

void FindMode::replaceNext()
{
    FindQueryUpdateStatus status = updateFindQuery(true);
    if (status == Invalid)
        return;
    if (status == Unchanged) {
        TextSelection current(view()->textArea()->editor()->selectionStart(),
                view()->textArea()->editor()->selectionEnd());
        if (_findIndex >= 0 && _findIndex < _findHits.size() && current == _findHits[_findIndex].selection) {
            // TODO: should we set the cursor to be just after the replacement?
            view()->buffer()->parseReplacement(Replacement(current,
                    QString::fromStdWString(_findHits[_findIndex].match.format(_replaceField->text().toStdWString()))));
            // TODO: modify the findbuffer appropriately instead of doing plainText() again in the next findNext call
            _findBufferDirty = true;
            // there shouldn't be any interactivity here
            status = updateFindQuery(true);
        }
    }
    findNextWithOptions(true, status);
}

void FindMode::replaceAll()
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
    view()->buffer()->parseReplacement(_replaces);
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

void FindMode::onReplaceFromTopDialogFinished(bb::system::SystemUiResult::Type type)
{
    if (type == bb::system::SystemUiResult::ConfirmButtonSelection) {
        view()->buffer()->parseReplacement(_replaces);
        Utility::dialog(tr("OK"), tr("Replace finished"),
                tr("%n occurrence(s) replaced.", "", _numberOfReplacesTillBottom+_replaces.size()));
    }
    _replaces.clear();
}

void FindMode::setLocked(bool locked)
{
    _findField->setEnabled(!locked);
    _replaceField->setEnabled(!locked);
    _goToFindFieldAction->setEnabled(!locked);
    _findPrevAction->setEnabled(!locked);
    _findNextAction->setEnabled(!locked);
    _replaceNextAction->setEnabled(!locked);
    _replaceAllAction->setEnabled(!locked);
    _undoAction->setEnabled(!locked && view()->buffer()->hasUndo());
    _redoAction->setEnabled(!locked && view()->buffer()->hasRedo());
    _findCancelAction->setEnabled(!locked);
}

void FindMode::onTranslatorChanged()
{
    _findField->setHintText(tr("Find text"));
    _replaceField->setHintText(tr("Replace with"));
    // actions
    _goToFindFieldAction->setTitle(tr("Go to find field"));
    _findPrevAction->setTitle(tr("Find previous"));
    _findNextAction->setTitle(tr("Find next"));
    _replaceNextAction->setTitle(tr("Replace next"));
    _replaceAllAction->setTitle(tr("Replace all remaining"));
    _findCancelAction->setTitle(tr("Cancel"));
    _undoAction->setTitle(tr("Undo"));
    _redoAction->setTitle(tr("Redo"));
    // options
    _regexOption->setTitle(tr("Regex"));
    _ignoreCaseOption->setTitle(tr("Ignore case"));
    _exactMatchOption->setTitle(tr("Exact match"));
}
