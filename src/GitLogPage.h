/*
 * GitLogPage.h
 *
 *  Created on: Aug 18, 2015
 *      Author: lingnan
 */

#ifndef GITLOGPAGE_H_
#define GITLOGPAGE_H_

#include <bb/cascades/ListItemProvider>
#include <bb/cascades/DataModel>
#include <libqgit2/qgitref.h>
#include <libqgit2/qgitcommit.h>
#include <PushablePage.h>

namespace bb {
    namespace cascades {
        class ListView;
        class ActionItem;
    }
}

class GitRepoPage;

class GitLogPage : public PushablePage
{
    Q_OBJECT
public:
    GitLogPage(GitRepoPage *);
    // this should always be a valid reference
    void setReference(const LibQGit2::Reference &);
    void resetReference();
    Q_SLOT void showCommitInfoSelection();
    Q_SLOT void checkoutSelection();
    void onTranslatorChanged();
Q_SIGNALS:
    void translatorChanged();
private:
    void reloadTitle();
    GitRepoPage *_repoPage;
    LibQGit2::Reference _reference;
    class CommitDataModel : public bb::cascades::DataModel {
    public:
        typedef QList<LibQGit2::Commit> Commits;
        typedef QList<Commits> DateSortedCommits;
        int childCount(const QVariantList &);
        bool hasChildren(const QVariantList &);
        QString itemType(const QVariantList &);
        QVariant data(const QVariantList &);
        void setCommits(const DateSortedCommits &);
        void clear();
    private:
        DateSortedCommits _commits;
    } _commitDataModel;
    class CommitItemProvider : public bb::cascades::ListItemProvider {
    public:
        CommitItemProvider(GitLogPage *page): _page(page) {}
        bb::cascades::VisualNode *createItem(bb::cascades::ListView *list, const QString &type);
        void updateItem(bb::cascades::ListView *list, bb::cascades::VisualNode *listItem, const QString &type,
            const QVariantList &indexPath, const QVariant &data);
    private:
        GitLogPage *_page;
    } _commitItemProvider;
    bb::cascades::ListView *_commitList;
    bb::cascades::ActionItem *_commitInfoCheckoutAction;

    bb::cascades::ActionItem *commitInfoCheckoutAction();
    Q_SLOT void commitInfoPageCheckout();
    Q_SLOT void reloadCommitInfoCheckoutActionTitle();
    Q_SLOT void showCommitInfoIndexPath(const QVariantList &);
};

#endif /* GITLOGPAGE_H_ */
