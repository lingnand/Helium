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
#include <bb/cascades/NavigationPane>
#include <bb/cascades/ActionItem>
#include <bb/cascades/TitleBar>
#include <bb/cascades/Shortcut>
#include <bb/cascades/KeyEvent>
#include <bb/cascades/TextFieldTitleBarKindProperties>
#include <RunProfile.h>
#include <WebRunProfile.h>
#include <RunProfileManager.h>
#include <NormalMode.h>
#include <Utility.h>
#include <ModKeyListener.h>
#include <Buffer.h>
#include <View.h>
#include <Filetype.h>
#include <FilePropertiesPage.h>
#include <MultiViewPane.h>
#include <Project.h>
#include <Helium.h>
#include <AppearanceSettings.h>
#include <ShortcutHelp.h>
#include <GitRepoPage.h>

using namespace bb::cascades;

NormalMode::NormalMode(View *view):
    ViewMode(view),
    _active(false), _lastFocused(false), _modifyingTextField(false),
    _runProfile(NULL),
    _textAreaModKeyListener(ModKeyListener::create(KEYCODE_RETURN)
        .onModifiedKeyReleased(this, SLOT(onTextAreaModifiedKey(bb::cascades::KeyEvent*, ModKeyListener*)))
        .onModKeyPressedAndReleased(this, SLOT(onTextAreaModKey(bb::cascades::KeyEvent*)))
        .onTextAreaInputModeChanged(view->textArea(), SLOT(setInputMode(bb::cascades::TextAreaInputMode::Type)))
        .modOffOn(view->textArea(), SIGNAL(focusedChanged(bool)))),
    _saveAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_save.png"))
        .addShortcut(Shortcut::create().key("s"))
        .onTriggered(view, SLOT(save()))),
    _saveAsAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_save_as.png"))
        .addShortcut(Shortcut::create().key("Shift+s"))
        .onTriggered(view, SLOT(saveAs()))),
    _openAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_open.png"))
        .addShortcut(Shortcut::create().key("e"))
        .onTriggered(view, SLOT(open()))),
    _undoAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_undo.png"))
        .addShortcut(Shortcut::create().key("z"))
        .onTriggered(view, SLOT(undo()))),
    _redoAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_redo.png"))
        .addShortcut(Shortcut::create().key("y"))
        .onTriggered(view, SLOT(redo()))),
    _findAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_search.png"))
        .addShortcut(Shortcut::create().key("f"))
        .onTriggered(view, SLOT(setFindMode()))),
    _runAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_play.png"))
        .addShortcut(Shortcut::create().key("r"))
        .onTriggered(this, SLOT(run()))),
    _propertiesAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_properties.png"))
        .addShortcut(Shortcut::create().key("p"))
        .onTriggered(this, SLOT(showProperties()))),
    _cloneAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_copy_link.png"))
        .addShortcut(Shortcut::create().key("i"))
        .onTriggered(view, SLOT(clone()))),
    _renameAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_rename.png"))
        .addShortcut(Shortcut::create().key("h"))
        .onTriggered(this, SLOT(rename()))),
    _reloadAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_reload.png"))
        .addShortcut(Shortcut::create().key("l"))
        .onTriggered(view, SLOT(reload()))),
    _gitAction(ActionItem::create()
//        .imageSource(QUrl("asset:///images/ic_reload.png"))
        .addShortcut(Shortcut::create().key("g"))
        .onTriggered(this, SLOT(showGitRepo()))),
    _closeAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_clear.png"))
        .addShortcut(Shortcut::create().key("x"))
        .onTriggered(view, SLOT(close()))),
    _closeProjectAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_delete.png"))
        .addShortcut(Shortcut::create().key("Backspace"))
        .onTriggered(view, SLOT(closeProject()))),
    _propertiesPage(NULL), _gitRepoPage(NULL)
{
    view->textArea()->addKeyListener(_textAreaModKeyListener);

    TextFieldTitleBarKindProperties *titleBarProperties = new TextFieldTitleBarKindProperties;
    _titleField = titleBarProperties->textField();
    _titleField->setFocusPolicy(FocusPolicy::Touch);
    _titleField->setBackgroundVisible(true);
    _titleField->textStyle()->setBase(SystemDefaults::TextStyles::titleText());
    conn(_titleField, SIGNAL(textChanged(const QString &)),
        this, SLOT(onTitleFieldTextChanged(const QString &)));
    conn(_titleField, SIGNAL(focusedChanged(bool)),
        this, SLOT(onTitleFieldFocusedChanged(bool)));
    _titleField->addKeyListener(ModKeyListener::create(KEYCODE_RETURN)
        .onModifiedKeyReleased(this, SLOT(onTitleFieldModifiedKey(bb::cascades::KeyEvent*, ModKeyListener*)))
        .onModKeyPressedAndReleased(view->textArea(), SLOT(requestFocus()))
        .onTextFieldInputModeChanged(_titleField, SLOT(setInputMode(bb::cascades::TextFieldInputMode::Type)))
        .modOffOn(_titleField, SIGNAL(focusedChanged(bool))));
    // the default titlebar
    _titleBar = TitleBar::create(TitleBarKind::TextField)
        .kindProperties(titleBarProperties);

    conn(view, SIGNAL(hasUndosChanged(bool)), _undoAction, SLOT(setEnabled(bool)));
    conn(view, SIGNAL(hasRedosChanged(bool)), _redoAction, SLOT(setEnabled(bool)));

    onTranslatorChanged();
    conn(view, SIGNAL(translatorChanged()), this, SLOT(onTranslatorChanged()));

    conn(view, SIGNAL(outOfView()), _titleField, SLOT(loseFocus()));

    onBufferDirtyChanged(view->buffer()->dirty());
    conn(view, SIGNAL(bufferDirtyChanged(bool)),
            this, SLOT(onBufferDirtyChanged(bool)));

    onBufferNameChanged(view->buffer()->name());
    conn(view, SIGNAL(bufferNameChanged(const QString&)),
            this, SLOT(onBufferNameChanged(const QString&)))

    onBufferFilepathChanged(view->buffer()->filepath());
    conn(view, SIGNAL(bufferFilepathChanged(const QString&)),
            this, SLOT(onBufferFilepathChanged(const QString&)))

    onBufferFiletypeChanged(view->buffer()->filetype());
    conn(view, SIGNAL(bufferFiletypeChanged(Filetype*, Filetype*)),
            this, SLOT(onBufferFiletypeChanged(Filetype*, Filetype*)))

    conn(Helium::instance()->appearance(), SIGNAL(shouldHideTitleBarChanged(bool)),
            this, SLOT(resetTitleBar()));
}

void NormalMode::setRunProfile(RunProfile *profile)
{
    if (profile != _runProfile) {
        if (_runProfile) {
            _runProfile->disconnect(this);
            _runProfile->exit();
            _runProfile->deleteLater();
        }
        _runProfile = profile;
        reloadRunnable();
        if (_runProfile) {
            conn(_runProfile, SIGNAL(runnableChanged(bool)),
                this, SLOT(reloadRunnable()));
        }
    }
}

void NormalMode::run()
{
    if (_runProfile)
        _runProfile->run();
}

void NormalMode::showProperties()
{
    if (!_propertiesPage) {
        _propertiesPage = new FilePropertiesPage;
        conn(view(), SIGNAL(translatorChanged()),
                _propertiesPage, SLOT(onTranslatorChanged()));

        _propertiesPage->setFiletype(view()->buffer()->filetype());
        conn(view(), SIGNAL(bufferFiletypeChanged(Filetype*, Filetype*)),
                _propertiesPage, SLOT(setFiletype(Filetype*)));
        conn(_propertiesPage, SIGNAL(filetypeSelectionChanged(Filetype *)),
                view(), SLOT(setFiletype(Filetype *)));

        _propertiesPage->setAutodetectFiletypeChecked(
                view()->buffer()->autodetectFiletype());
        conn(view(), SIGNAL(bufferAutodetectFiletypeChanged(bool)),
                _propertiesPage, SLOT(setAutodetectFiletypeChecked(bool)));
        conn(_propertiesPage, SIGNAL(autodetectFiletypeCheckedChanged(bool)),
                view(), SLOT(setAutodetectFiletype(bool)));

        conn(_propertiesPage, SIGNAL(backButtonTriggered()),
                view()->content(), SLOT(pop()));
    }
    view()->content()->push(_propertiesPage);
}

void NormalMode::showGitRepo()
{
    if (!_gitRepoPage) {
        _gitRepoPage = new GitRepoPage(view()->project());
        conn(view()->content(), SIGNAL(popTransitionEnded(bb::cascades::Page*)),
            _gitRepoPage, SLOT(onPopTransitionEnded(bb::cascades::Page*)));
    }
    _gitRepoPage->reload();
    view()->content()->push(_gitRepoPage);
}

void NormalMode::resetTitleBar()
{
    if (_active)
        view()->setPageTitleBar(
            Helium::instance()->appearance()->shouldHideTitleBar() ? NULL : _titleBar);
}

void NormalMode::onTitleFieldTextChanged(const QString &text)
{
    _modifyingTextField = true;
    view()->setName(text);
    _modifyingTextField = false;
}

void NormalMode::onTextAreaModKey(KeyEvent *)
{
    view()->textArea()->editor()->insertPlainText("\n");
}

void NormalMode::onTextAreaModifiedKey(KeyEvent *event, ModKeyListener *listener)
{
    switch (event->keycap()) {
        case KEYCODE_BACKSPACE: {
            QList<ShortcutHelp> helps;
            QString prefix(RETURN_SYMBOL);
            helps << ShortcutHelp::fromActionItem(_saveAction, prefix)
                  << ShortcutHelp::fromActionItem(_undoAction, prefix)
                  << ShortcutHelp::fromActionItem(_redoAction, prefix)
                  << ShortcutHelp::fromActionItem(_openAction, prefix)
                  << ShortcutHelp::fromActionItem(_findAction, prefix)
                  << ShortcutHelp::fromActionItem(_runAction, prefix)
                  << ShortcutHelp::fromActionItem(_renameAction, prefix)
                  << ShortcutHelp::fromActionItem(_gitAction, prefix)
                  << ShortcutHelp::fromActionItem(_closeAction, prefix)
                  << ShortcutHelp("T", TAB_SYMBOL, prefix)
                  << ShortcutHelp("D", tr("Delete line"), prefix)
                  << ShortcutHelp("V", tr("Paste Clipboard"), prefix)
                  << ShortcutHelp(SPACE_SYMBOL, tr("Lose Focus"), prefix)
                  << ShortcutHelp::fromShortcut(view()->parent()->newViewShortcut(), prefix)
                  << ShortcutHelp::fromShortcut(view()->parent()->prevTabShortcut(), prefix)
                  << ShortcutHelp::fromShortcut(view()->parent()->nextTabShortcut(), prefix)
                  << ShortcutHelp(BACKSPACE_SYMBOL, tr("Display Shortcuts"), prefix);
            Utility::dialog(tr("Dismiss"), tr("Shortcuts"), ShortcutHelp::showAll(helps));
            break;
        }
        case KEYCODE_T: // Tab
            view()->textArea()->editor()->insertPlainText("\t");
            break;
        case KEYCODE_Z: // Z on most platforms
            view()->buffer()->undo();
            break;
        case KEYCODE_Y: // Redo
            view()->buffer()->redo();
            break;
        case KEYCODE_H: // Head
            rename();
            break;
        case KEYCODE_F: // Find
            view()->setFindMode();
            break;
        case KEYCODE_S: // Save
            if (view()->save() == View::OpenedFilePicker)
                listener->modOff();
            break;
        case KEYCODE_E: // Edit
            view()->open(); // this always opens some sort of UI
            listener->modOff();
            break;
        case KEYCODE_R: // Run
            run();
            break;
        case KEYCODE_D: // Delete
            view()->killCurrentLine();
            break;
        case KEYCODE_X: // Kill
            view()->close();
            break;
        case KEYCODE_G: // git
            showGitRepo();
            break;
        case KEYCODE_C: // Create
            view()->parent()->createEmptyView();
            break;
        case KEYCODE_Q:
            view()->parent()->setPrevTabActive();
            break;
        case KEYCODE_W:
            view()->parent()->setNextTabActive();
            break;
        default:
            Utility::handleBasicTextControlModifiedKey(view()->textArea()->editor(), event);
    }
}

void NormalMode::rename()
{
    if (_titleField->isEnabled()) {
        // make sure the title field is put into view
        view()->setPageTitleBar(_titleBar);
        _titleField->requestFocus();
    } else {
        Utility::toast(tr("Buffer name is locked"));
    }
}

void NormalMode::onBufferNameChanged(const QString &name)
{
    if (_modifyingTextField) {
        return;
    }
    if (!_titleField->isEnabled()) {
        // NOTE: hack to get around the problem of title field not grey
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
    bool empty = filepath.isEmpty();
    _titleField->setEnabled(empty);
    _renameAction->setEnabled(empty);
    _reloadAction->setEnabled(!empty);
}

void NormalMode::onBufferFiletypeChanged(Filetype *change, Filetype *old)
{
    if (old) {
        old->disconnect(this);
    }
    onRunProfileManagerChanged(change ? change->runProfileManager() : NULL);
    if (change) {
        conn(change, SIGNAL(runProfileManagerChanged(RunProfileManager*, RunProfileManager*)),
                this, SLOT(onRunProfileManagerChanged(RunProfileManager*)));
    }
}

void NormalMode::onRunProfileManagerChanged(RunProfileManager *runProfileManager)
{
    setRunProfile(runProfileManager ? runProfileManager->createRunProfile(view()) : NULL);
}

void NormalMode::autoFocus()
{
    // focus title text only when the text area is empty
    if (view()->page()->titleBar() == _titleBar
            && view()->buffer()->state().empty() && _titleField->isEnabled())
        _titleField->requestFocus();
    else
        view()->textArea()->requestFocus();
}

void NormalMode::onEnter()
{
    _active = true;

    view()->hideAllPageActions();

    reloadLocked();
    conn(view(), SIGNAL(bufferLockedChanged(bool)), this, SLOT(reloadLocked()));
    // disable page navigation when focusing the text area
    conn(view()->textArea(), SIGNAL(focusedChanged(bool)),
        view(), SLOT(blockPageKeyListener(bool)));

    view()->page()->addAction(_saveAction, ActionBarPlacement::Signature);
    view()->page()->addAction(_undoAction, ActionBarPlacement::OnBar);
    view()->page()->addAction(_redoAction, ActionBarPlacement::OnBar);
    view()->page()->addAction(_saveAsAction);
    view()->page()->addAction(_openAction);
    view()->page()->addAction(_findAction);
    view()->page()->addAction(_runAction);
    view()->page()->addAction(_propertiesAction);
    view()->page()->addAction(_cloneAction);
    view()->page()->addAction(_renameAction);
    view()->page()->addAction(_reloadAction);
    view()->page()->addAction(_gitAction);
    view()->page()->addAction(_closeAction);
    view()->page()->addAction(_closeProjectAction);
    resetTitleBar();

    view()->textArea()->setEditable(true);
    _textAreaModKeyListener->setEnabled(true);

    view()->textArea()->loseFocus();
    if (_lastFocused) {
        view()->textArea()->requestFocus();
    }
}

void NormalMode::onExit()
{
    _active = false;

    disconn(view(), SIGNAL(bufferLockedChanged(bool)), this, SLOT(reloadLocked()));
    disconn(view()->textArea(), SIGNAL(focusedChanged(bool)),
        view(), SLOT(blockPageKeyListener(bool)));
    _textAreaModKeyListener->setEnabled(false);
    _lastFocused = view()->textArea()->isFocused();
}

void NormalMode::onBufferDirtyChanged(bool dirty)
{
    _titleField->textStyle()->setFontStyle(dirty ? FontStyle::Italic : FontStyle::Normal);
}

void NormalMode::onTitleFieldFocusedChanged(bool focused)
{
    view()->blockPageKeyListener(focused);
    if (!focused)
        resetTitleBar();
}

void NormalMode::onTitleFieldModifiedKey(bb::cascades::KeyEvent *event, ModKeyListener *)
{
    switch (event->keycap()) {
        case KEYCODE_BACKSPACE: {
            QList<ShortcutHelp> helps;
            QString prefix(RETURN_SYMBOL);
            helps << ShortcutHelp("V", tr("Paste Clipboard"), prefix)
                  << ShortcutHelp(SPACE_SYMBOL, tr("Lose Focus"), prefix)
                  << ShortcutHelp::fromShortcut(view()->parent()->newViewShortcut(), prefix)
                  << ShortcutHelp::fromShortcut(view()->parent()->prevTabShortcut(), prefix)
                  << ShortcutHelp::fromShortcut(view()->parent()->nextTabShortcut(), prefix)
                  << ShortcutHelp(BACKSPACE_SYMBOL, tr("Display Shortcuts"), prefix);
            Utility::dialog(tr("Dismiss"), tr("Shortcuts"), ShortcutHelp::showAll(helps));
            break;
        }
        case KEYCODE_C: // Create
            view()->parent()->createEmptyView();
            break;
        case KEYCODE_Q:
            view()->parent()->setPrevTabActive();
            break;
        case KEYCODE_W:
            view()->parent()->setNextTabActive();
            break;
        default:
            Utility::handleBasicTextControlModifiedKey(_titleField->editor(), event);
    }
}

void NormalMode::reloadLocked()
{
    bool locked = view()->buffer()->locked();
    view()->textArea()->setEditable(!locked);
    bool pathEmpty = view()->buffer()->filepath().isEmpty();
    _titleField->setEnabled(!locked && pathEmpty);
    _saveAction->setEnabled(!locked);
    _saveAsAction->setEnabled(!locked);
    _openAction->setEnabled(!locked);
    _undoAction->setEnabled(!locked && view()->buffer()->hasUndo());
    _redoAction->setEnabled(!locked && view()->buffer()->hasRedo());
    _findAction->setEnabled(!locked);
    _cloneAction->setEnabled(!locked);
    _renameAction->setEnabled(!locked && pathEmpty);
    _reloadAction->setEnabled(!locked && !pathEmpty);
    _gitAction->setEnabled(!locked);
    _closeAction->setEnabled(!locked);
    _closeProjectAction->setEnabled(!locked);
    reloadRunnable();
}

void NormalMode::reloadRunnable()
{
    _runAction->setEnabled(!view()->buffer()->locked() && _runProfile && _runProfile->runnable());
}

void NormalMode::onTranslatorChanged()
{
    _titleBar->setTitle(tr("Editor"));
    _titleField->setHintText(tr("Name"));
    _saveAction->setTitle(tr("Save"));
    _saveAsAction->setTitle(tr("Save As"));
    _openAction->setTitle(tr("Open"));
    _undoAction->setTitle(tr("Undo"));
    _redoAction->setTitle(tr("Redo"));
    _findAction->setTitle(tr("Find"));
    _runAction->setTitle(tr("Run"));
    _propertiesAction->setTitle(tr("Properties"));
    _cloneAction->setTitle(tr("Clone"));
    _renameAction->setTitle(tr("Rename"));
    _reloadAction->setTitle(tr("Reload"));
    _gitAction->setTitle(tr("Git"));
    _closeAction->setTitle(tr("Close"));
    _closeProjectAction->setTitle(tr("Close Project"));
    if (_propertiesPage)
        _propertiesPage->onTranslatorChanged();
    if (_gitRepoPage)
        _gitRepoPage->onTranslatorChanged();
}
