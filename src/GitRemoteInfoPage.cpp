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
#include <GitBranchPage.h>
#include <Segment.h>
#include <LocaleAwareActionItem.h>
#include <Defaults.h>
#include <Utility.h>

using namespace bb::cascades;

GitRemoteInfoPage::GitRemoteInfoPage(GitBranchPage *page):
    _branchPage(page),
    _remote(NULL),
    _nameHeader(Header::create()),
    _nameField(TextField::create()
        .inputFlags(Defaults::codeInputFlags())
        .contentFlags(Defaults::codeContentFlags())),
    _urlHeader(Header::create()),
    _urlField(TextField::create()
        .inputFlags(Defaults::codeInputFlags())
        .contentFlags(Defaults::codeContentFlags())),
    _saveRemoteAction(NULL)
{
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

void GitRemoteInfoPage::setRemote(LibQGit2::Remote *remote)
{
    if (_remote != remote) {
        _remote = remote;
        // update the infos
        if (_remote) {
            _nameField->setText(_remote->name());
            _nameField->setEnabled(false);
            _urlField->setText(_remote->url());
        } else {
            _nameField->resetText();
            _nameField->setEnabled(true);
            _urlField->resetText();
        }
    }
}

void GitRemoteInfoPage::setActions(Actions actions)
{
    while (actionCount() > 0)
        removeAction(actionAt(0));
    if (actions.testFlag(SaveRemote)) {
        if (!_saveRemoteAction)
            _saveRemoteAction = LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Save Remote"))
                .reloadTitleOn(this, SIGNAL(translatorChanged()))
                .addShortcut(Shortcut::create().key("s"))
                .onTriggered(this, SLOT(saveRemote()));
        addAction(_saveRemoteAction, ActionBarPlacement::Signature);
    }
}

void GitRemoteInfoPage::saveRemote()
{
    _branchPage->createRemote(_nameField->text(), _urlField->text());
    // keep the nameField and urlField content for reuse
}

void GitRemoteInfoPage::setRemoteUrl(const QString &url)
{
    if (_remote) {
        _remote->setUrl(url);
        _remote->save();
        _branchPage->reload();
    }
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
