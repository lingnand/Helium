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
#include <libqgit2/qgitrepository.h>
#include <libqgit2/qgitremote.h>
#include <GitBranchPage.h>
#include <GitRepoPage.h>
#include <GitLogPage.h>
#include <Utility.h>

using namespace bb::cascades;

GitBranchPage::GitBranchPage(GitRepoPage *page):
    _repoPage(page),
    _dataModel(this, page->repo()),
    _itemProvider(this),
    _branchList(ListView::create()
        .dataModel(&_dataModel)
        .listItemProvider(&_itemProvider))
{
    conn(_branchList, SIGNAL(triggered(QVariantList)),
        this, SLOT(showBranchLogIndexPath(const QVariantList &)));
    setTitleBar(TitleBar::create());
    setContent(_branchList);

    onTranslatorChanged(false);
}

void GitBranchPage::showBranchLogIndexPath(const QVariantList &ip)
{
    if (ip.size() < 2)
        return;
    _repoPage->pushLogPage(_dataModel.data(ip).value<LibQGit2::Reference>());
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

void GitBranchPage::reload()
{
    _dataModel.reload();
}

void GitBranchPage::reset()
{
    _dataModel.clear();
}

void GitBranchPage::onTranslatorChanged(bool reload)
{
    PushablePage::onTranslatorChanged();
    titleBar()->setTitle(tr("Branches"));
    if (reload)
        this->reload();
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

void GitBranchPage::onRemoteTransferProgress(int progress)
{
    qDebug() << "transfer progress changed" << progress;
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

VisualNode *GitBranchPage::BranchItemProvider::createItem(ListView *list, const QString &type)
{
    if (type == "localHeader")
        return Header::create().mode(HeaderMode::Interactive);
    if (type == "localItem")
        // TODO: add actionset
        return StandardListItem::create();
    if (type == "remoteHeader") {
        RemoteHeader *header = new RemoteHeader;
        header->setMode(HeaderMode::Interactive);
        header->setSubtitle(tr("Info"));
        conn(header, SIGNAL(clicked()),
            _page, SLOT(showRemoteInfo()));
        return header;
    }
    if (type == "remoteItem")
        // TODO: add actionset
        return StandardListItem::create();
}

void GitBranchPage::BranchItemProvider::updateItem(ListView *list, VisualNode *item, const QString &type,
    const QVariantList &ip, const QVariant &data)
{
    if (ip.empty())
        return;
    switch (ip[0].toInt()) {
        case 0: {
            if (ip.size() == 1) {
                ((Header *) item)->setTitle(data.toString());
            } else {
                StandardListItem *it = (StandardListItem *) item;
                const LibQGit2::Reference ref = data.value<LibQGit2::Reference>();
                it->setTitle(ref.branchName());
                if (ref.branchIsHead())
                    it->setStatus(tr("HEAD"));
            }
            break;
        }
        default: {
            if (ip.size() == 1) {
                RemoteHeader *it = (RemoteHeader *) item;
                it->setTitle(data.value<LibQGit2::Remote *>()->name());
                it->setIndexPath(ip);
            } else {
                StandardListItem *it = (StandardListItem *) item;
                const LibQGit2::Reference ref = data.value<LibQGit2::Reference>();
                it->setTitle(ref.branchName());
            }
            break;
        }
    }
}
