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
#include <libqgit2/qgitrepository.h>
#include <libqgit2/qgitremote.h>
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
        .addShortcut(Shortcut::create().key("a"))
        .onTriggered(this, SLOT(addBranch()))),
    _dataModel(this, page->repo()),
    _itemProvider(this),
    _branchList(ListView::create()
        .dataModel(&_dataModel)
        .listItemProvider(&_itemProvider)),
    _progressIndicator(new AutoHideProgressIndicator)
{
    conn(_branchList, SIGNAL(triggered(QVariantList)),
        this, SLOT(showBranchLogIndexPath(const QVariantList &)));
    setTitleBar(TitleBar::create());
    setContent(Container::create()
        .add(_branchList)
        .add(_progressIndicator));
    addAction(_addBranchAction, ActionBarPlacement::Signature);

    onTranslatorChanged(false);
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
    _repoPage->disconnect(this);
}

void GitBranchPage::showBranchLogIndexPath(const QVariantList &ip)
{
    if (ip.size() < 2)
        return;
    _repoPage->pushLogPage(_dataModel.data(ip).value<LibQGit2::Reference>());
}

void GitBranchPage::showBranchLogSelection()
{
    showBranchLogIndexPath(_branchList->selected());
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
    _tempTarget = _dataModel.data(_branchList->selected()).value<LibQGit2::Reference>();
    Utility::dialog(tr("Continue"), tr("Cancel"), tr("Confirm deletion"),
            tr("Are you sure you want to delete branch %1?")
                .arg(_tempTarget.branchName()),
            this, SLOT(onDeleteBranchDialogFinished(bb::system::SystemUiResult::Type)));
}

void GitBranchPage::onDeleteBranchDialogFinished(bb::system::SystemUiResult::Type type)
{
    if (type == bb::system::SystemUiResult::ConfirmButtonSelection)
        _repoPage->deleteBranch(_tempTarget);
    _tempTarget = LibQGit2::Reference();
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
            _dataModel.data(selection).value<LibQGit2::Reference>().branchName()
                .split('/').last());
}

void GitBranchPage::pullBranchSelection()
{
    qDebug() << "PULLING BRANCH for" << _branchList->selected();
}

void GitBranchPage::pushBranchSelection()
{
    qDebug() << "PUSHING BRANCH for" << _branchList->selected();
}

void GitBranchPage::showRemoteInfo()
{
    const QVariantList &ip = ((BranchItemProvider::RemoteHeader *) sender())->indexPath();
    LibQGit2::Remote *remote = _dataModel.data(ip).value<LibQGit2::Remote *>();
    if (!remote) {
        qWarning() << "What? no remote to show for index path" << ip;
        return;
    }
    qDebug() << "SHOWING REMOTE INFO for" << remote->name();
}

void GitBranchPage::addBranch()
{
    Utility::prompt(tr("Continue"), tr("Cancel"),
            tr("Add Branch"), tr("Enter the branch name to add"),
            QString(), tr("branch name"), this,
            SLOT(onAddBranchPromptFinished(bb::system::SystemUiResult::Type, const QString&)));
}

void GitBranchPage::onAddBranchPromptFinished(bb::system::SystemUiResult::Type type, const QString &text)
{
    if (type == bb::system::SystemUiResult::ConfirmButtonSelection)
        _repoPage->createBranch(text);
}

void GitBranchPage::reload()
{
    _dataModel.reload();
}

void GitBranchPage::reset()
{
    _dataModel.clear();
}

void GitBranchPage::onRemoteTransferProgress(int progress)
{
    qDebug() << "transfer progress changed" << progress;
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

// NOTE: this function safeguards against invalid ips (unlike many other datamodel)
// reason: GitBranchPage might access data using stale ips (in showRemoteInfo)
QVariant GitBranchPage::BranchDataModel::data(const QVariantList &ip)
{
    if (ip.empty())
        return QVariant();
    int i0 = ip[0].toInt();
    switch (i0) {
        case 0:
            if (ip.size() == 1)
                return tr("Local"); // plain header with no configuration
            return QVariant::fromValue(_localBranches.value(ip[1].toInt()));
        default: {
            const RemoteInfo ri = _remoteInfos.value(i0-1);
            if (ip.size() == 1)
                return QVariant::fromValue(ri.remote);
            return QVariant::fromValue(ri.branches.value(ip[1].toInt()));
        }
    }
    return QVariant();
}

void GitBranchPage::BranchDataModel::reload()
{
    clear();
    // load all the remotes
    QStringList remotes = _repo->listRemotes();
    for (int i = 0; i < remotes.size(); ++i) {
        // TODO: use proper credentials
        LibQGit2::Remote *remote = _repo->remote(remotes[i], LibQGit2::Credentials(), this);
        conn(remote, SIGNAL(transferProgress(int)),
                _page, SLOT(onRemoteTransferProgress(int)));
        _remoteInfos.append(RemoteInfo(remote));
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
        return Header::create().mode(HeaderMode::Interactive);
    if (type == "localItem")
        return StandardListItem::create()
            .actionSet(ActionSet::create()
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "View Log"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .onTriggered(_page, SLOT(showBranchLogSelection())))
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Checkout"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .onTriggered(_page, SLOT(checkoutBranchSelection())))
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Merge"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .onTriggered(_page, SLOT(mergeBranchSelection())))
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Rebase"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .onTriggered(_page, SLOT(rebaseBranchSelection())))
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Delete"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .onTriggered(_page, SLOT(deleteBranchSelection()))));
    if (type == "remoteHeader") {
        RemoteHeader *header = new RemoteHeader;
        header->setMode(HeaderMode::Interactive);
        header->setSubtitle(tr("More"));
        conn(header, SIGNAL(clicked()),
            _page, SLOT(showRemoteInfo()));
        return header;
    }
    if (type == "remoteItem")
        // TODO: add actionset
        return StandardListItem::create()
            .actionSet(ActionSet::create()
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "View Log"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .onTriggered(_page, SLOT(showBranchLogSelection())))
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Checkout"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .onTriggered(_page, SLOT(checkoutBranchSelection())))
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Merge"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .onTriggered(_page, SLOT(mergeBranchSelection())))
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Rebase"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .onTriggered(_page, SLOT(rebaseBranchSelection())))
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Fetch"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .onTriggered(_page, SLOT(fetchBranchSelection())))
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Pull"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                    .onTriggered(_page, SLOT(pullBranchSelection())))
                .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Push"))
                    .reloadTitleOn(_page, SIGNAL(translatorChanged()))
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
        RemoteHeader *it = (RemoteHeader *) item;
        it->setTitle(data.value<LibQGit2::Remote *>()->name());
        it->setIndexPath(ip);
    } else if (type == "remoteItem") {
        StandardListItem *it = (StandardListItem *) item;
        const LibQGit2::Reference ref = data.value<LibQGit2::Reference>();
        it->setTitle(ref.branchName());
    }
}
