/*
 * GitLogPage.cpp
 *
 *  Created on: Aug 18, 2015
 *      Author: lingnan
 */

#include <bb/cascades/TitleBar>
#include <bb/cascades/ListView>
#include <bb/cascades/Header>
#include <bb/cascades/StandardListItem>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/ActionItem>
#include <bb/cascades/Shortcut>
#include <bb/cascades/ActionSet>
#include <libqgit2/qgitrevwalk.h>
#include <libqgit2/qgitrepository.h>
#include <libqgit2/qgitexception.h>
#include <GitLogPage.h>
#include <GitRepoPage.h>
#include <GitCommitInfoPage.h>
#include <LocaleAwareActionItem.h>
#include <Utility.h>

using namespace bb::cascades;

GitLogPage::GitLogPage(GitRepoPage *repoPage):
    _repoPage(repoPage),
    _remote(NULL),
    _commitItemProvider(this),
    _commitList(ListView::create()
        .dataModel(&_commitDataModel)
        .listItemProvider(&_commitItemProvider)),
    _checkoutBranchAction(NULL),
    _deleteBranchAction(NULL),
    _mergeBranchAction(NULL),
    _rebaseBranchAction(NULL),
    _fetchBranchAction(NULL),
    _pullBranchAction(NULL),
    _pushBranchAction(NULL)
{
    conn(_commitList, SIGNAL(triggered(QVariantList)),
        this, SLOT(showCommitInfoIndexPath(const QVariantList &)));
    setTitleBar(TitleBar::create());
    setContent(_commitList);

    onTranslatorChanged();
}

void GitLogPage::setReference(const LibQGit2::Reference &reference, LibQGit2::Remote *remote)
{
    _reference = reference;
    _remote = remote;
    try {
        LibQGit2::RevWalk walk(*_repoPage->repo());
        walk.setSorting(LibQGit2::RevWalk::Topological |
                LibQGit2::RevWalk::Time);
        walk.push(_reference);
        // get all the commits and set to commitDataModel
        GitLogPage::CommitDataModel::DateSortedCommits commits;
        LibQGit2::Commit commit;
        QDate last, current;
        int i = 0;
        while (walk.next(commit)) {
            current = commit.dateTime().date();
            if (current != last) {
                last = current;
                commits.append(GitLogPage::CommitDataModel::Commits());
            }
            commits.last().append(commit);
        }
        _commitDataModel.setCommits(commits);
    } catch (const LibQGit2::Exception &e) {
        qDebug() << "::::LIBQGIT2 ERROR when loading commits::::" << e.what();
    }
    reloadTitle();
}

void GitLogPage::resetReference()
{
    _commitDataModel.clear();
    _reference = LibQGit2::Reference();
    _remote = NULL;
}

void GitLogPage::setActions(Actions actions)
{
    while (actionCount() > 0)
        removeAction(actionAt(0));
    QList<ActionItem *> list;
    if (actions.testFlag(PullBranch)) {
        if (!_pullBranchAction)
            _pullBranchAction = LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Pull Branch"))
                .reloadTitleOn(this, SIGNAL(translatorChanged()))
                .addShortcut(Shortcut::create().key("u"))
                .onTriggered(this, SLOT(pullBranch()));
        list.append(_pullBranchAction);
    }
    if (actions.testFlag(FetchBranch)) {
        if (!_fetchBranchAction)
            _fetchBranchAction = LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Fetch Branch"))
                .reloadTitleOn(this, SIGNAL(translatorChanged()))
                .addShortcut(Shortcut::create().key("f"))
                .onTriggered(this, SLOT(fetchBranch()));
        list.append(_fetchBranchAction);
    }
    if (actions.testFlag(PushBranch)) {
        if (!_pushBranchAction)
            _pushBranchAction = LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Push to Branch"))
                .reloadTitleOn(this, SIGNAL(translatorChanged()))
                .addShortcut(Shortcut::create().key("p"))
                .onTriggered(this, SLOT(pushBranch()));
        list.append(_pushBranchAction);
    }
    if (actions.testFlag(CheckoutBranch)) {
        if (!_checkoutBranchAction)
            _checkoutBranchAction = LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Checkout Branch"))
                .reloadTitleOn(this, SIGNAL(translatorChanged()))
                .addShortcut(Shortcut::create().key("c"))
                .onTriggered(this, SLOT(checkoutBranch()));
        list.append(_checkoutBranchAction);
    }
    if (actions.testFlag(MergeBranch)) {
        if (!_mergeBranchAction)
            _mergeBranchAction = LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Merge Branch"))
                .reloadTitleOn(this, SIGNAL(translatorChanged()))
                .addShortcut(Shortcut::create().key("m"))
                .onTriggered(this, SLOT(mergeBranch()));
        list.append(_mergeBranchAction);
    }
    if (actions.testFlag(RebaseBranch)) {
        if (!_rebaseBranchAction)
            _rebaseBranchAction = LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Rebase Branch"))
                .reloadTitleOn(this, SIGNAL(translatorChanged()))
                .addShortcut(Shortcut::create().key("r"))
                .onTriggered(this, SLOT(rebaseBranch()));
        list.append(_rebaseBranchAction);
    }
    if (actions.testFlag(DeleteBranch)) {
        if (!_deleteBranchAction)
            _deleteBranchAction = LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Delete Branch"))
                .reloadTitleOn(this, SIGNAL(translatorChanged()))
                .addShortcut(Shortcut::create().key("Backspace"))
                .onTriggered(this, SLOT(deleteBranch()));
        list.append(_deleteBranchAction);
    }
    for (int i = 0; i < list.size(); i++) {
        switch (i) {
            case 0:
                addAction(list[i], ActionBarPlacement::Signature);
                break;
            case 1: case 2:
                addAction(list[i], ActionBarPlacement::OnBar);
                break;
            default:
                addAction(list[i]);
        }
    }
}

void GitLogPage::checkoutBranch()
{
    _repoPage->checkoutBranch(_reference);
    pop();
}

void GitLogPage::deleteBranch()
{
    _repoPage->safeDeleteBranch(_reference);
    pop();
}

void GitLogPage::mergeBranch()
{
    _repoPage->merge(_reference);
    pop();
}

void GitLogPage::rebaseBranch()
{
    _repoPage->rebase(_reference);
    pop();
}

void GitLogPage::fetchBranch()
{
    _repoPage->fetch(_remote, _reference);
    pop();
}

void GitLogPage::pullBranch()
{
    _repoPage->pull(_remote, _reference);
    pop();
}

void GitLogPage::pushBranch()
{
    _repoPage->safePush(_remote, _reference.branchName().split('/').last());
    pop();
}

void GitLogPage::reloadTitle()
{
    if (_reference.isNull()) {
        titleBar()->setTitle(tr("No Log"));
    } else {
        // set the title
        titleBar()->setTitle(_reference.name());
    }
}

void GitLogPage::showCommitInfoSelection()
{
    showCommitInfoIndexPath(_commitList->selected());
}

void GitLogPage::showCommitInfoIndexPath(const QVariantList &ip)
{
    if (ip.size() < 2)
        return;
    _repoPage->pushCommitInfoPage(_commitDataModel.data(ip).value<LibQGit2::Commit>(),
            GitCommitInfoPage::Checkout);
}

void GitLogPage::checkoutSelection()
{
    _repoPage->checkoutCommit(_commitDataModel
            .data(_commitList->selected()).value<LibQGit2::Commit>());
    pop(); // pop to repo page
}

void GitLogPage::onTranslatorChanged()
{
    PushablePage::onTranslatorChanged();
    reloadTitle();
    emit translatorChanged();
}

int GitLogPage::CommitDataModel::childCount(const QVariantList &ip)
{
    if (ip.empty())
        return _commits.size();
    if (ip.size() == 1)
        return _commits[ip[0].toInt()].size();
    return 0;
}

bool GitLogPage::CommitDataModel::hasChildren(const QVariantList &ip)
{
    if (ip.empty())
        return true;
    if (ip.size() == 1)
        return ip[0].toInt() < _commits.size();
    return false;
}

QString GitLogPage::CommitDataModel::itemType(const QVariantList &ip)
{
    if (ip.size() == 1)
        return "header";
    return "item";
}

QVariant GitLogPage::CommitDataModel::data(const QVariantList &ip)
{
    switch (ip.size()) {
        case 1:
            return _commits[ip[0].toInt()].first().dateTime().date();
        case 2:
            return QVariant::fromValue(_commits[ip[0].toInt()][ip[1].toInt()]);
    }
    return QVariant();
}

void GitLogPage::CommitDataModel::setCommits(const GitLogPage::CommitDataModel::DateSortedCommits &commits)
{
    _commits = commits;
    emit itemsChanged(DataModelChangeType::Init);
}

void GitLogPage::CommitDataModel::clear()
{
    _commits.clear();
}

VisualNode *GitLogPage::CommitItemProvider::createItem(ListView *, const QString &type)
{
    if (type == "header")
        return Header::create();
    return StandardListItem::create()
        .actionSet(ActionSet::create()
            .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "View Info"))
                .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                .onTriggered(_page, SLOT(showCommitInfoSelection())))
            .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Checkout"))
                .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                .onTriggered(_page, SLOT(checkoutSelection()))));
}

void GitLogPage::CommitItemProvider::updateItem(ListView *, bb::cascades::VisualNode *listItem, const QString &type,
    const QVariantList &ip, const QVariant &data)
{
    if (type == "header") {
        ((Header *) listItem)->setTitle(data.toDate().toString());
    } else if (type == "item" && ip.size() > 1) {
        StandardListItem *li = (StandardListItem *) listItem;
        LibQGit2::Commit commit = data.value<LibQGit2::Commit>();
        li->setTitle(commit.message());
        li->setDescription(commit.author().name());
        li->setStatus(commit.dateTime().time().toString());
    }
}
