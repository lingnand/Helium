/*
 * GitBranchPage.cpp
 *
 *  Created on: Aug 28, 2015
 *      Author: lingnan
 */

#include <QObjectList>
#include <bb/cascades/ListView>
#include <bb/cascades/TitleBar>
#include <bb/cascades/StandardListItem>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/ActionSet>
#include <bb/cascades/Container>
#include <bb/cascades/Shortcut>
#include <bb/cascades/ContextMenuHandler>
#include <libqgit2/qgitrepository.h>
#include <libqgit2/qgitremote.h>
#include <Helium.h>
#include <GitSettings.h>
#include <GitBranchPage.h>
#include <GitRepoPage.h>
#include <GitLogPage.h>
#include <LocaleAwareActionItem.h>
#include <AutoHideProgressIndicator.h>
#include <Utility.h>

using namespace bb::cascades;

GitBranchPage::GitBranchPage(GitRepoPage *page):
    _repoPage(page),
    _addBranchAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_git_branch_add.png"))
        .addShortcut(Shortcut::create().key("a"))
        .onTriggered(this, SLOT(addBranch()))),
    _addRemoteAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_git_remote_add.png"))
        .addShortcut(Shortcut::create().key("m"))
        .onTriggered(this, SLOT(addRemote()))),
    _reloadAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_reload.png"))
        .addShortcut(Shortcut::create().key("l"))
        .onTriggered(this, SLOT(reload()))),
    _dataModel(this, page->repo()),
    _itemProvider(this),
    _branchList(ListView::create()
        .scrollRole(ScrollRole::Main)
        .dataModel(&_dataModel)
        .listItemProvider(&_itemProvider)),
    _progressIndicator(new AutoHideProgressIndicator),
    _tempRemote(NULL)
{
    conn(_branchList, SIGNAL(triggered(QVariantList)),
        this, SLOT(onBranchListTriggered(const QVariantList &)));
    setTitleBar(TitleBar::create());
    setContent(Container::create()
        .add(_branchList)
        .add(_progressIndicator));
    addAction(_addBranchAction, ActionBarPlacement::Signature);
    addAction(_addRemoteAction, ActionBarPlacement::OnBar);
    addAction(_reloadAction);

    onTranslatorChanged(false);

    conn(Helium::instance()->git(), SIGNAL(sshCredentialsChanged(const LibQGit2::Credentials&)),
        this, SLOT(reload()));
}

void GitBranchPage::connectToRepoPage()
{
    conn(_repoPage, SIGNAL(progressChanged(float, bb::cascades::ProgressIndicatorState::Type)),
        _progressIndicator, SLOT(displayProgress(float, bb::cascades::ProgressIndicatorState::Type)));
    conn(_repoPage, SIGNAL(progressDismissed()),
        _progressIndicator, SLOT(hide()));
    onGitRepoPageInProgressChanged(_repoPage->inProgress());
    conn(_repoPage, SIGNAL(inProgressChanged(bool)),
        this, SLOT(onGitRepoPageInProgressChanged(bool)));
}

void GitBranchPage::disconnectFromRepoPage()
{
    _repoPage->disconnect(_progressIndicator);
    disconn(_repoPage, SIGNAL(inProgressChanged(bool)),
        this, SLOT(onGitRepoPageInProgressChanged(bool)));
}

void GitBranchPage::onBranchListTriggered(const QVariantList &ip)
{
    if (ip.empty())
        return;
    switch (ip[0].toInt()) {
        case 0:
            if (ip.size() > 1) {
                const LibQGit2::Reference &ref = _dataModel.data(ip).value<LibQGit2::Reference>();
                if (ref.branchIsHead())
                    _repoPage->pushLogPage(ref);
                else
                    _repoPage->pushLogPage(ref, NULL,
                            GitLogPage::CheckoutBranch |
                            GitLogPage::MergeBranch |
                            GitLogPage::RebaseBranch |
                            GitLogPage::DeleteBranch);
            }
            break;
        default:
            if (ip.size() == 1)
                showRemoteInfo(ip);
            else
                _repoPage->pushLogPage(_dataModel.data(ip).value<LibQGit2::Reference>(),
                        _dataModel.data(QVariantList() << ip[0]).value<LibQGit2::Remote *>(),
                        GitLogPage::CheckoutBranch |
                        GitLogPage::MergeBranch |
                        GitLogPage::RebaseBranch |
                        GitLogPage::PullBranch |
                        GitLogPage::FetchBranch |
                        GitLogPage::PushBranch);
            break;
    }
}

void GitBranchPage::onRemoteHeaderContextMenuVisualStateChanged(ContextMenuVisualState::Type type)
{
    // XXX: when it reaches VisibleCompact we would be getting the current selection
    // a bit of a hack
    if (type == ContextMenuVisualState::VisibleCompact) {
        QVariantList selection = _branchList->selected();
        if (selection.size() == 1 && selection[0].toInt() > 0) {
            for (int i = _dataModel.childCount(selection); i >= 0; i--)
                _branchList->select(QVariantList(selection) << i);
        }
    }
}

void GitBranchPage::showInfoRemoteSelection()
{
    showRemoteInfo(_branchList->selected());
}

void GitBranchPage::showRemoteInfo(const QVariantList &ip)
{
    LibQGit2::Remote *remote = _dataModel.data(ip).value<LibQGit2::Remote *>();
    if (!remote) {
        qWarning() << "What? no remote for ip" << ip;
        return;
    }
    _repoPage->pushRemoteInfoPage(GitRemoteInfoPage::DisplayRemote, remote);
}

void GitBranchPage::refreshRemoteSelection()
{
    LibQGit2::Remote *remote = _dataModel.data(_branchList->selected())
            .value<LibQGit2::Remote *>();
    if (!remote) {
        qWarning() << "What? no remote for selection" << _branchList->selected();
        return;
    }
    _repoPage->fetchBaseAndPrune(remote);
}

void GitBranchPage::pushToBranchRemoteSelection()
{
    LibQGit2::Remote *remote = _dataModel.data(_branchList->selected())
            .value<LibQGit2::Remote *>();
    if (!remote) {
        qWarning() << "What? no remote for selection" << _branchList->selected();
        return;
    }
    _tempRemote = remote;
    Utility::prompt(tr("Push"), tr("Cancel"),
            tr("Push to Branch"), tr("Enter the branch name to push to"),
            _repoPage->repo()->head().branchName(), tr("Branch name"), this,
            SLOT(onPushToBranchPromptFinished(bb::system::SystemUiResult::Type, const QString&)));
}

void GitBranchPage::onPushToBranchPromptFinished(bb::system::SystemUiResult::Type type, const QString &branch)
{
    if (_tempRemote && type == bb::system::SystemUiResult::ConfirmButtonSelection)
        _repoPage->push(_tempRemote, branch);
    _tempRemote = NULL;
}

void GitBranchPage::showBranchLogSelection()
{
    onBranchListTriggered(_branchList->selected());
}

void GitBranchPage::checkoutBranchSelection()
{
    _repoPage->checkoutBranch(_dataModel.data(_branchList->selected())
            .value<LibQGit2::Reference>());
}

void GitBranchPage::mergeBranchSelection()
{
    _repoPage->merge(_dataModel.data(_branchList->selected())
            .value<LibQGit2::Reference>());
    // no change in branch; no need to reload
}

void GitBranchPage::rebaseBranchSelection()
{
    _repoPage->rebase(_dataModel.data(_branchList->selected())
            .value<LibQGit2::Reference>());
    // no change in branch; no need to reload
}

void GitBranchPage::deleteBranchSelection()
{
    _repoPage->safeDeleteBranch(_dataModel.data(_branchList->selected()).value<LibQGit2::Reference>());
}

void GitBranchPage::fetchBranchSelection()
{
    QVariantList selection = _branchList->selected();
    Q_ASSERT(selection.size() == 2);
    LibQGit2::Remote *remote = _dataModel.data(QVariantList() << selection[0])
            .value<LibQGit2::Remote *>();
    if (!remote) {
        qWarning() << "What? no remote for selection" << selection;
        return;
    }
    _repoPage->fetch(remote,
            _dataModel.data(selection).value<LibQGit2::Reference>());
}

void GitBranchPage::pullBranchSelection()
{
    QVariantList selection = _branchList->selected();
    Q_ASSERT(selection.size() == 2);
    LibQGit2::Remote *remote = _dataModel.data(QVariantList() << selection[0])
            .value<LibQGit2::Remote *>();
    if (!remote) {
        qWarning() << "What? no remote for selection" << selection;
        return;
    }
    _repoPage->pull(remote,
            _dataModel.data(selection).value<LibQGit2::Reference>());
}

void GitBranchPage::pushBranchSelection()
{
    QVariantList selection = _branchList->selected();
    Q_ASSERT(selection.size() == 2);
    LibQGit2::Remote *remote = _dataModel.data(QVariantList() << selection[0])
            .value<LibQGit2::Remote *>();
    if (!remote) {
        qWarning() << "What? no remote for selection" << selection;
        return;
    }
    _repoPage->safePush(remote, _dataModel.data(selection)
                .value<LibQGit2::Reference>().branchName().split('/').last());
}

void GitBranchPage::addBranch()
{
    Utility::prompt(tr("Continue"), tr("Cancel"),
            tr("Add Branch"), tr("Enter the branch name to add"),
            QString(), tr("Branch name"), this,
            SLOT(onAddBranchPromptFinished(bb::system::SystemUiResult::Type, const QString&)));
}

void GitBranchPage::onAddBranchPromptFinished(bb::system::SystemUiResult::Type type, const QString &text)
{
    if (type == bb::system::SystemUiResult::ConfirmButtonSelection)
        _repoPage->createBranch(text);
}

void GitBranchPage::addRemote()
{
    _repoPage->pushRemoteInfoPage(GitRemoteInfoPage::SaveRemote);
}

void GitBranchPage::reload()
{
    _dataModel.reload();
}

void GitBranchPage::reset()
{
    _dataModel.clear();
}

void GitBranchPage::onGitRepoPageInProgressChanged(bool inProgress)
{
    if (inProgress) {
        // lock the content
        _branchList->setEnabled(false);
        for (int i = 0; i < actionCount(); i++)
            actionAt(i)->setEnabled(false);
    } else {
        // unlock the content
        _branchList->setEnabled(true);
        for (int i = 0; i < actionCount(); i++)
            actionAt(i)->setEnabled(true);
        reload();
    }
}

void GitBranchPage::onTranslatorChanged(bool reload)
{
    PushablePage::onTranslatorChanged();
    titleBar()->setTitle(tr("Branches"));
    _addBranchAction->setTitle(tr("Add Branch"));
    _addRemoteAction->setTitle(tr("Add Remote"));
    _reloadAction->setTitle(tr("Reload"));
    if (reload)
        this->reload();
    emit translatorChanged();
}

int GitBranchPage::BranchDataModel::childCount(const QVariantList &ip)
{
    if (ip.empty())
        return _remoteInfos.size()+1;
    if (ip.size() == 1) {
        int index = ip[0].toInt();
        if (index == 0)
            return _localBranches.size();
        return _remoteInfos[index-1].branches.size();
    }
    return 0;
}

bool GitBranchPage::BranchDataModel::hasChildren(const QVariantList &ip)
{
    if (ip.empty())
        return true;
    if (ip.size() == 1)
        return ip[0].toInt() < _remoteInfos.size()+1;
    return false;
}

QString GitBranchPage::BranchDataModel::itemType(const QVariantList &ip)
{
    if (ip.empty())
        return QString();
    switch (ip[0].toInt()) {
        case 0:
            if (ip.size() == 1)
                return "localHeader"; // plain header with no configuration
            return "localItem"; // item with 'view log', 'delete', 'checkout', 'merge', 'rebase'
        default:
            if (ip.size() == 1)
                return "remoteHeader";  // header which can be tapped to bring up config
            return "remoteItem"; // item with 'view log', 'checkout', 'merge', 'rebase', 'fetch', 'pull', 'push'
    }
}

QVariant GitBranchPage::BranchDataModel::data(const QVariantList &ip)
{
    if (ip.empty())
        return QVariant();
    int i0 = ip[0].toInt();
    switch (i0) {
        case 0:
            if (ip.size() == 1)
                return tr("Local"); // plain header with no configuration
            return QVariant::fromValue(_localBranches[ip[1].toInt()]);
        default: {
            const RemoteInfo ri = _remoteInfos.value(i0-1);
            if (ip.size() == 1)
                return QVariant::fromValue(ri.remote);
            return QVariant::fromValue(ri.branches[ip[1].toInt()]);
        }
    }
    return QVariant();
}

void GitBranchPage::BranchDataModel::reload()
{
    clear();
    // load all the remotes
    QStringList remotes = _repo->listRemotes();
    if (!remotes.isEmpty()) {
        const LibQGit2::Credentials &cred = Helium::instance()->git()->sshCredentials();
        for (int i = 0; i < remotes.size(); ++i) {
            _remoteInfos.append(RemoteInfo(_repo->remote(remotes[i], cred, this)));
        }
    }
    // load all the branches
    QList<LibQGit2::Repository::Branch> branches = _repo->listBranches(LibQGit2::Repository::BranchAll);
    for (int i = 0; i < branches.size(); ++i) {
        const LibQGit2::Repository::Branch &branch = branches[i];
        switch (branch.type) {
            case LibQGit2::Repository::BranchLocal:
                _localBranches.append(branch.reference);
                goto ContinueBranchLoop;
            case LibQGit2::Repository::BranchRemote: {
                QString branchName(branch.reference.name());
                // search for a matching remote
                for (int j = 0; j < _remoteInfos.size(); ++j) {
                    RemoteInfo &ri = _remoteInfos[j];
                    for (size_t k = 0, count = ri.remote->refspecCount(); k < count; ++k) {
                        if (ri.remote->refspec(k).destinationMatches(branchName)) {
                            ri.branches.append(branch.reference);
                            goto ContinueBranchLoop;
                        }
                    }
                }
                qWarning() << "Error: unable to find a remote for branch" << branchName;
            }
        }
    ContinueBranchLoop:
        ;
    }
    emit itemsChanged(DataModelChangeType::Init);
}

void GitBranchPage::BranchDataModel::clear()
{
    _localBranches.clear();
    _remoteInfos.clear();
    // delete all the children (Remote *)
    for (int i = children().size()-1; i >= 0; --i) {
        children()[i]->deleteLater();
    }
}

VisualNode *GitBranchPage::BranchItemProvider::createItem(ListView *, const QString &type)
{
    if (type == "localHeader")
        return Header::create();
    if (type == "localItem")
        return StandardListItem::create()
            .actionSet(ActionSet::create()
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "View Log"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .imageSource(QUrl("asset:///images/ic_git_log.png"))
                    .onTriggered(_page, SLOT(showBranchLogSelection())))
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Checkout"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .imageSource(QUrl("asset:///images/ic_git_checkout.png"))
                    .onTriggered(_page, SLOT(checkoutBranchSelection())))
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Merge"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .imageSource(QUrl("asset:///images/ic_git_merge.png"))
                    .onTriggered(_page, SLOT(mergeBranchSelection())))
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Rebase"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .imageSource(QUrl("asset:///images/ic_git_rebase.png"))
                    .onTriggered(_page, SLOT(rebaseBranchSelection())))
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Delete"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .imageSource(QUrl("asset:///images/ic_delete.png"))
                    .onTriggered(_page, SLOT(deleteBranchSelection()))));
    if (type == "remoteHeader") {
        return Header::create()
            .subtitle(tr("Info >"))
            .actionSet(ActionSet::create()
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "View Info"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .imageSource(QUrl("asset:///images/ic_view_details.png"))
                    .onTriggered(_page, SLOT(showInfoRemoteSelection())))
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Refresh"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .imageSource(QUrl("asset:///images/ic_reload.png"))
                    .onTriggered(_page, SLOT(refreshRemoteSelection())))
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Push to Branch..."))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .imageSource(QUrl("asset:///images/ic_git_push.png"))
                    .onTriggered(_page, SLOT(pushToBranchRemoteSelection()))))
            .contextMenuHandler(ContextMenuHandler::create()
                .onVisualStateChanged(_page, SLOT(onRemoteHeaderContextMenuVisualStateChanged(bb::cascades::ContextMenuVisualState::Type))));
    }
    if (type == "remoteItem")
        // TODO: add actionset
        return StandardListItem::create()
            .actionSet(ActionSet::create()
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "View Log"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .imageSource(QUrl("asset:///images/ic_git_log.png"))
                    .onTriggered(_page, SLOT(showBranchLogSelection())))
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Checkout"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .imageSource(QUrl("asset:///images/ic_git_checkout.png"))
                    .onTriggered(_page, SLOT(checkoutBranchSelection())))
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Merge"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .imageSource(QUrl("asset:///images/ic_git_merge.png"))
                    .onTriggered(_page, SLOT(mergeBranchSelection())))
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Rebase"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .imageSource(QUrl("asset:///images/ic_git_rebase.png"))
                    .onTriggered(_page, SLOT(rebaseBranchSelection())))
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Fetch"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .imageSource(QUrl("asset:///images/ic_git_fetch.png"))
                    .onTriggered(_page, SLOT(fetchBranchSelection())))
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Pull"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .imageSource(QUrl("asset:///images/ic_git_pull.png"))
                    .onTriggered(_page, SLOT(pullBranchSelection())))
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Push"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .imageSource(QUrl("asset:///images/ic_git_push.png"))
                    .onTriggered(_page, SLOT(pushBranchSelection()))));
    return NULL;
}

void GitBranchPage::BranchItemProvider::updateItem(ListView *list, VisualNode *item, const QString &type,
    const QVariantList &ip, const QVariant &data)
{
    if (type == "localHeader")
        ((Header *) item)->setTitle(data.toString());
    else if (type == "localItem") {
        StandardListItem *it = (StandardListItem *) item;
        const LibQGit2::Reference ref = data.value<LibQGit2::Reference>();
        it->setTitle(ref.branchName());
        bool isHead = ref.branchIsHead();
        if (isHead) {
            it->setStatus(tr("HEAD"));
        } else {
            it->resetStatus();
        }
        ActionSet *set = it->actionSetAt(0);
        // XXX: the first action would be view log,
        // and all the others are ones that don't apply to head
        for (int i = 1; i < set->count(); i++) {
            set->at(i)->setEnabled(!isHead);
        }
    } else if (type == "remoteHeader") {
        Header *it = (Header *) item;
        it->setTitle(data.value<LibQGit2::Remote *>()->name());
    } else if (type == "remoteItem") {
        StandardListItem *it = (StandardListItem *) item;
        const LibQGit2::Reference ref = data.value<LibQGit2::Reference>();
        it->setTitle(ref.branchName().split('/').last());
    }
}
