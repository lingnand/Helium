/*
 * NormalMode.cpp
 *
 *  Created on: May 3, 2015
 *      Author: lingnan
 */

#include <bb/cascades/SystemDefaults>
#include <bb/cascades/FontStyle>
#include <bb/cascades/TextField>
#include <bb/cascades/TextArea>
#include <bb/cascades/Page>
#include <bb/cascades/ActionItem>
#include <bb/cascades/TitleBar>
#include <bb/cascades/Shortcut>
#include <bb/cascades/KeyEvent>
#include <bb/cascades/TextFieldTitleBarKindProperties>
#include <NormalMode.h>
#include <Utility.h>
#include <ModKeyListener.h>
#include <Buffer.h>
#include <View.h>

using namespace bb::cascades;

NormalMode::NormalMode(View *view): ViewMode(view), _saveAction(NULL)
{
    TextFieldTitleBarKindProperties *titleBarProperties = new TextFieldTitleBarKindProperties;
    _titleField = titleBarProperties->textField();
    _titleField->setFocusPolicy(FocusPolicy::Touch);
    _titleField->setBackgroundVisible(true);
    _titleField->textStyle()->setBase(SystemDefaults::TextStyles::titleText());
    conn(_titleField, SIGNAL(focusedChanged(bool)),
        this, SLOT(onTitleFieldFocusChanged(bool)));
    _titleField->addKeyListener(ModKeyListener::create(KEYCODE_RETURN)
        .onModifiedKeyPressed(this, SLOT(onTitleFieldModifiedKeyPressed(bb::cascades::KeyEvent*, ModKeyListener*)))
        .onModKeyPressed(view->textArea(), SLOT(requestFocus()))
        .onTextFieldInputModeChanged(_titleField, SLOT(setInputMode(bb::cascades::TextFieldInputMode::Type)))
        .modOffOn(_titleField, SIGNAL(focusedChanged(bool))));
    // the default titlebar
    _titleBar = TitleBar::create(TitleBarKind::TextField)
        .kindProperties(titleBarProperties);

    // text
    onTranslatorChanged();
    conn(view, SIGNAL(translatorChanged()), this, SLOT(onTranslatorChanged()));

    onBufferDirtyChanged(view->buffer()->dirty());
    conn(view, SIGNAL(bufferDirtyChanged(bool)),
            this, SLOT(onBufferDirtyChanged(bool)));

    onBufferNameChanged(view->buffer()->name());
    conn(view, SIGNAL(bufferNameChanged(const QString&)),
            this, SLOT(onBufferNameChanged(const QString&)))

    onBufferFilepathChanged(view->buffer()->filepath());
    conn(view, SIGNAL(bufferFilepathChanged(const QString&)),
            this, SLOT(onBufferFilepathChanged(const QString&)))
}

TitleBar *NormalMode::titleBar() const
{
    return _titleBar;
}

TextField *NormalMode::titleField() const
{
    return _titleField;
}

void NormalMode::onBufferNameChanged(const QString &name)
{
    if (!_titleField->isEnabled()) {
        // TODO: hack to get around the problem of title field not grey
        _titleField->setEnabled(true);
        _titleField->setText(name);
        _titleField->setEnabled(false);
    } else {
        _titleField->setText(name);
    }
}

void NormalMode::onBufferFilepathChanged(const QString &filepath)
{
    qDebug() << "filepath set to" << filepath;
    _titleField->setEnabled(filepath.isEmpty());
}

void NormalMode::autoFocus(bool goToModeControl)
{
    // focus title text only when the text area is empty
    if (goToModeControl || view()->buffer()->state().empty())
        _titleField->requestFocus();
    else
        view()->textArea()->requestFocus();
}

void NormalMode::onEnter(bool hasPreviousMode)
{
    view()->content()->removeAllActions();
    _saveAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_save.png"))
        .addShortcut(Shortcut::create().key("s"))
        .onTriggered(view(), SLOT(save()));
    _saveAsAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_save_as.png"))
        .onTriggered(view(), SLOT(saveAs()));
    _openAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_open.png"))
        .addShortcut(Shortcut::create().key("e"))
        .onTriggered(view(), SLOT(open()));
    _undoAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_undo.png"))
        .addShortcut(Shortcut::create().key("z"))
        .onTriggered(view(), SIGNAL(undo()));
    conn(view(), SIGNAL(hasUndosChanged(bool)), _undoAction, SLOT(setEnabled(bool)));
    _redoAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_redo.png"))
        .addShortcut(Shortcut::create().key("r"))
        .onTriggered(view(), SIGNAL(redo()));
    conn(view(), SIGNAL(hasRedosChanged(bool)), _redoAction, SLOT(setEnabled(bool)));
    _findAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_search.png"))
        .addShortcut(Shortcut::create().key("f"))
        .onTriggered(view(), SLOT(setFindMode()));
    _cloneAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_copy_link.png"))
        .addShortcut(Shortcut::create().key("y"))
        .onTriggered(view(), SLOT(clone()));
    _closeAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_clear.png"))
        .addShortcut(Shortcut::create().key("x"))
        .onTriggered(view(), SLOT(close()));
    reloadActionTitles();

    setLocked(view()->buffer()->locked());
    conn(view(), SIGNAL(bufferLockedChanged(bool)), this, SLOT(setLocked(bool)));
    conn(view(), SIGNAL(outOfView()), view()->textArea(), SLOT(loseFocus()));

    view()->content()->addAction(_saveAction, ActionBarPlacement::Signature);
    view()->content()->addAction(_undoAction, ActionBarPlacement::OnBar);
    view()->content()->addAction(_redoAction, ActionBarPlacement::OnBar);
    view()->content()->addAction(_saveAsAction);
    view()->content()->addAction(_openAction);
    view()->content()->addAction(_findAction);
    view()->content()->addAction(_cloneAction);
    view()->content()->addAction(_closeAction);
    view()->content()->setTitleBar(_titleBar);

    view()->textArea()->setEditable(true);
    view()->textAreaModKeyListener()->setEnabled(true);

    if (hasPreviousMode) {
        view()->textArea()->loseFocus();
        view()->textArea()->requestFocus();
    }
}

void NormalMode::onExit()
{
    disconn(view(), SIGNAL(bufferLockedChanged(bool)), this, SLOT(setLocked(bool)));
    disconn(view(), SIGNAL(outOfView()), view()->textArea(), SLOT(loseFocus()));
    _saveAction = NULL;
}

void NormalMode::onBufferDirtyChanged(bool dirty)
{
    _titleField->textStyle()->setFontStyle(dirty ? FontStyle::Italic : FontStyle::Normal);
}

void NormalMode::onTitleFieldFocusChanged(bool focus)
{
    if (!focus) {
        // the user defocused the text field
        // set the buffer name
        view()->buffer()->setName(_titleField->text());
    }
}

void NormalMode::onTitleFieldModifiedKeyPressed(bb::cascades::KeyEvent *event, ModKeyListener *)
{
    view()->handleTextControlBasicModifiedKeys(_titleField->editor(), event);
}

void NormalMode::setLocked(bool locked)
{
    view()->textArea()->setEditable(!locked);
    _titleField->setEnabled(!locked && view()->buffer()->filepath().isEmpty());
    if (_saveAction) {
        _saveAction->setEnabled(!locked);
        _saveAsAction->setEnabled(!locked);
        _openAction->setEnabled(!locked);
        _undoAction->setEnabled(!locked && view()->buffer()->hasUndo());
        _redoAction->setEnabled(!locked && view()->buffer()->hasRedo());
        _findAction->setEnabled(!locked);
        _cloneAction->setEnabled(!locked);
        _closeAction->setEnabled(!locked);
    }
}

void NormalMode::onTranslatorChanged()
{
    _titleField->setHintText(tr("Enter the title"));
    if (_saveAction)
        reloadActionTitles();
}

void NormalMode::reloadActionTitles()
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
