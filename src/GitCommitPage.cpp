/*
 * GitCommitPage.cpp
 *
 *  Created on: Aug 15, 2015
 *      Author: lingnan
 */

#include <bb/cascades/Shortcut>
#include <bb/cascades/TitleBar>
#include <bb/cascades/TextArea>
#include <bb/cascades/Shortcut>
#include <bb/cascades/ActionItem>
#include <bb/cascades/KeyEvent>
#include <GitCommitPage.h>
#include <GitRepoPage.h>
#include <Helium.h>
#include <AppearanceSettings.h>
#include <GitSettings.h>
#include <ModKeyListener.h>
#include <ShortcutHelp.h>
#include <Utility.h>

using namespace bb::cascades;

GitCommitPage::GitCommitPage(GitRepoPage *repoPage):
    _repoPage(repoPage),
    _textArea(TextArea::create()
        .contentFlags(TextContentFlag::ActiveTextOff)
        .focusPolicy(FocusPolicy::Touch)),
    _settingsAction(ActionItem::create()
        .addShortcut(Shortcut::create().key("s"))
        .onTriggered(this, SLOT(showSigSettings())))
{
    AppearanceSettings *appearance = Helium::instance()->appearance();
    _textArea->textStyle()->setFontFamily(appearance->fontFamily());
    conn(appearance, SIGNAL(fontFamilyChanged(const QString&)),
        _textArea->textStyle(), SLOT(setFontFamily(const QString&)));
    _textArea->textStyle()->setFontSize(appearance->fontSize());
    conn(appearance, SIGNAL(fontSizeChanged(bb::cascades::FontSize::Type)),
        _textArea->textStyle(), SLOT(setFontSize(bb::cascades::FontSize::Type)));

    GitSettings *git = Helium::instance()->git();
    setTitleBar(TitleBar::create()
        .title(git->name())
        .acceptAction(ActionItem::create()
            .addShortcut(Shortcut::create().key("c"))
            .onTriggered(this, SLOT(commit())))
        .dismissAction(ActionItem::create()
        .onTriggered(this, SLOT(pop()))));
    conn(git, SIGNAL(nameChanged(const QString&)),
        titleBar(), SLOT(setTitle(const QString&)));
    _textArea->addKeyListener(ModKeyListener::create(KEYCODE_RETURN)
        .onModifiedKeyReleased(this, SLOT(onTextAreaModifiedKey(bb::cascades::KeyEvent*)))
        .onModKeyPressedAndReleased(this, SLOT(onTextAreaModKey(bb::cascades::KeyEvent*)))
        .onTextAreaInputModeChanged(_textArea, SLOT(setInputMode(bb::cascades::TextAreaInputMode::Type)))
        .modOffOn(_textArea, SIGNAL(focusedChanged(bool))));
    setContent(_textArea);
    addAction(_settingsAction);
    setActionBarVisibility(ChromeVisibility::Compact);
    onTranslatorChanged();
}

void GitCommitPage::focus()
{
    _textArea->requestFocus();
}

void GitCommitPage::setHintMessage(const QString &hint)
{
    _textArea->setHintText(hint);
}

void GitCommitPage::commit()
{
    _repoPage->commit(_textArea->text());
    // TODO: maybe put up a progressIndicator and wait for progress
    // finish from GitRepoPage?
    _textArea->resetText();
    pop();
}

void GitCommitPage::showSigSettings()
{

}

void GitCommitPage::onTextAreaModKey(bb::cascades::KeyEvent *)
{
    _textArea->editor()->insertPlainText("\n");
}

void GitCommitPage::onTextAreaModifiedKey(bb::cascades::KeyEvent *event)
{
    switch (event->keycap()) {
        case KEYCODE_BACKSPACE: {
            QList<ShortcutHelp> helps;
            QString prefix(RETURN_SYMBOL);
            helps << ShortcutHelp("C", tr("Commit"), prefix)
                  << ShortcutHelp("X", tr("Cancel"), prefix)
                  << ShortcutHelp(SPACE_SYMBOL, tr("Lose Focus"), prefix)
                  << ShortcutHelp("V", tr("Paste Clipboard"), prefix)
                  << ShortcutHelp(BACKSPACE_SYMBOL, tr("Display Shortcuts"), prefix);
            Utility::dialog(tr("Dismiss"), tr("Shortcuts"), ShortcutHelp::showAll(helps));
            break;
        }
        case KEYCODE_C:
            commit();
            break;
        case KEYCODE_X:
            pop();
            break;
        default:
            Utility::handleBasicTextControlModifiedKey(_textArea->editor(), event);
    }
}

void GitCommitPage::onTranslatorChanged()
{
    PushablePage::onTranslatorChanged();
    titleBar()->acceptAction()->setTitle(tr("Commit"));
    titleBar()->dismissAction()->setTitle(tr("Cancel"));
    _settingsAction->setTitle(tr("Settings"));
}

