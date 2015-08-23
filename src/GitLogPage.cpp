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
#include <libqgit2/qgitrevwalk.h>
#include <libqgit2/qgitrepository.h>
#include <libqgit2/qgitexception.h>
#include <GitLogPage.h>
#include <Utility.h>

using namespace bb::cascades;

GitLogPage::GitLogPage(LibQGit2::Repository *repo):
    _repo(repo)
{
    ListView *list = ListView::create()
        .dataModel(&_commitDataModel)
        .listItemProvider(&_commitItemProvider);
    conn(list, SIGNAL(triggered(QVariantList)),
        this, SLOT(showCommitIndexPath(const QVariantList &)));
    setTitleBar(TitleBar::create());
    setContent(list);

    onTranslatorChanged();
}

void GitLogPage::setReference(const LibQGit2::Reference &reference)
{
    _reference = reference;
    try {
        LibQGit2::RevWalk walk(*_repo);
        walk.setSorting(LibQGit2::RevWalk::Topological |
                LibQGit2::RevWalk::Time);
        walk.push(_reference);
        // get all the commits and set to commitDataModel
        GitLogPage::CommitDataModel::DateSortedCommits commits;
        LibQGit2::Commit commit;
        QDate last, current;
        int i = 0;
        qDebug() << "START loading commits";
        while (walk.next(commit)) {
            qDebug("=== Loading commit %d ===", i++);
            qDebug() << "message" << commit.message();
            current = commit.dateTime().date();
            if (current != last) {
                last = current;
                commits.append(GitLogPage::CommitDataModel::Commits());
            }
            commits.last().append(commit);
        }
        qDebug() << "FINISHED loading commits";
        _commitDataModel.setCommits(commits);
    } catch (const LibQGit2::Exception &e) {
        qDebug() << "::::LIBQGIT2 ERROR when loading commits::::" << e.what();
    }
    reloadTitle();
}

void GitLogPage::resetReference()
{
    _reference = LibQGit2::Reference();
    _commitDataModel.clear();
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

void GitLogPage::showCommitIndexPath(const QVariantList &indexPath)
{
    // push a dedicated commit page
    qDebug() << "SHOW commit for index path:" << indexPath;
}

void GitLogPage::onTranslatorChanged()
{
    PushablePage::onTranslatorChanged();
    reloadTitle();
}

int GitLogPage::CommitDataModel::childCount(const QVariantList &ip)
{
    if (ip.empty())
        return _commits.size();
    if (ip.size() == 1) {
        return _commits[ip[0].toInt()].size();
    }
    return 0;
}

bool GitLogPage::CommitDataModel::hasChildren(const QVariantList &ip)
{
    if (ip.empty())
        return true;
    if (ip.size() == 1) {
        if (ip[0].toInt() < _commits.size())
            return true;
    }
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
    qDebug() << "set commits of length" << _commits.size();
    for (int i = 0; i < _commits.size(); i++) {
        qDebug() << "commits" << i << "has length" << _commits[i].size();
    }
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
    return StandardListItem::create();
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
        // TODO: actionSet
        // add checkout action
    }
}
