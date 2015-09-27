/*
 * GitRemoteInfoPage.cpp
 *
 *  Created on: Sep 24, 2015
 *      Author: lingnan
 */

#include <libqgit2/qgitremote.h>
#include <bb/cascades/Header>
#include <bb/cascades/TextField>
#include <bb/cascades/TitleBar>
#include <bb/cascades/ScrollView>
#include <bb/cascades/Shortcut>
#include <GitRemoteInfoPage.h>
#include <GitRepoPage.h>
#include <Segment.h>
#include <LocaleAwareActionItem.h>
#include <Defaults.h>
#include <Utility.h>

using namespace bb::cascades;

GitRemoteInfoPage::GitRemoteInfoPage(GitRepoPage *page):
    _mode(DisplayRemote),
    _repoPage(page),
    _remote(NULL),
    _nameHeader(Header::create()),
    _nameField(TextField::create()
        .inputFlags(Defaults::codeInputFlags())
        .contentFlags(Defaults::codeContentFlags())),
    _urlHeader(Header::create()),
    _urlField(TextField::create()
        .inputMode(TextFieldInputMode::Url)
        .inputFlags(Defaults::codeInputFlags())
        .contentFlags(Defaults::codeContentFlags())),
    _saveRemoteAction(NULL),
    _cloneAction(NULL)
{
    _urlField->input()->setSubmitKey(SubmitKey::Done);
    _urlField->input()->setSubmitKeyFocusBehavior(SubmitKeyFocusBehavior::Lose);
    conn(_urlField, SIGNAL(textChanged(const QString&)),
        this, SLOT(setRemoteUrl(const QString&)));
    setTitleBar(TitleBar::create());
    setContent(ScrollView::create(Segment::create().section()
            .add(_nameHeader)
            .add(Segment::create().subsection().add(_nameField))
            .add(_urlHeader)
            .add(Segment::create().subsection().add(_urlField))));

    onTranslatorChanged();
}

void GitRemoteInfoPage::resetRemote()
{
    _remote = NULL;
}

void GitRemoteInfoPage::setMode(Mode mode, LibQGit2::Remote *remote)
{
    if (mode != _mode) {
        _mode = mode;
        while (actionCount() > 0)
            removeAction(actionAt(0));
        switch (mode) {
            case SaveRemote:
                _remote = NULL;
                _nameField->setEnabled(true);
                _nameField->resetText();
                _urlField->resetText();
                if (!_saveRemoteAction)
                    _saveRemoteAction = LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Save Remote"))
                        .reloadTitleOn(this, SIGNAL(translatorChanged()))
                        .imageSource(QUrl("asset:///images/ic_save.png"))
                        .addShortcut(Shortcut::create().key("s"))
                        .onTriggered(this, SLOT(saveRemote()));
                addAction(_saveRemoteAction, ActionBarPlacement::Signature);
                break;
            case Clone:
                _remote = NULL;
                _nameField->setEnabled(false);
                _nameField->setText("origin");
                _urlField->resetText();
                if (!_cloneAction)
                    _cloneAction = LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Clone"))
                        .reloadTitleOn(this, SIGNAL(translatorChanged()))
                        .imageSource(QUrl("asset:///images/ic_git_clone.png"))
                        .addShortcut(Shortcut::create().key("c"))
                        .onTriggered(this, SLOT(clone()));
                addAction(_cloneAction, ActionBarPlacement::Signature);
                break;
        }
    }
    if (_mode == DisplayRemote) {
        _remote = remote;
        if (_remote) {
            _nameField->setEnabled(false);
            _nameField->setText(_remote->name());
            _urlField->setText(_remote->url());
        }
    }
}

void GitRemoteInfoPage::clone()
{
    _repoPage->clone(_urlField->text());
    pop();
}

void GitRemoteInfoPage::saveRemote()
{
    _repoPage->createRemote(_nameField->text(), _urlField->text());
    // keep the nameField and urlField content for reuse
    pop();
}

void GitRemoteInfoPage::setRemoteUrl(const QString &url)
{
    if (_remote)
        _repoPage->setRemoteUrl(_remote, url);
}

void GitRemoteInfoPage::onTranslatorChanged()
{
    PushablePage::onTranslatorChanged();
    titleBar()->setTitle(tr("Remote Info"));
    _nameHeader->setTitle(tr("Name"));
    _nameField->setHintText(tr("Name"));
    _urlHeader->setTitle(tr("URL for Fetching/Pushing"));
    _urlField->setHintText(tr("URL"));
}
