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

namespace LibQGit2 {
    class Remote;
}

class GitRepoPage;

class GitLogPage : public PushablePage
{
    Q_OBJECT
public:
    GitLogPage(GitRepoPage *);
    // this should always be a valid reference
    void setReference(const LibQGit2::Reference &, LibQGit2::Remote *remote=NULL);
    void resetReference();
    enum Action {
        CheckoutBranch = 1u << 0,
        MergeBranch = 1u << 1,
        RebaseBranch = 1u << 2,
        DeleteBranch = 1u << 3,
        FetchBranch = 1u << 4,
        PullBranch = 1u << 5,
        PushBranch = 1u << 6,
    };
    Q_DECLARE_FLAGS(Actions, Action)
    void setActions(Actions=Actions());
    Q_SLOT void showCommitInfoSelection();
    Q_SLOT void checkoutSelection();
    Q_SLOT void onTranslatorChanged();
Q_SIGNALS:
    void translatorChanged();
private:
    void reloadTitle();
    GitRepoPage *_repoPage;
    LibQGit2::Reference _reference;
    LibQGit2::Remote *_remote; // optional remote for a remote branch
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
    bb::cascades::ActionItem *_checkoutBranchAction, *_deleteBranchAction;
    bb::cascades::ActionItem *_mergeBranchAction, *_rebaseBranchAction;
    bb::cascades::ActionItem *_fetchBranchAction, *_pullBranchAction;
    bb::cascades::ActionItem *_pushBranchAction;

    Q_SLOT void showCommitInfoIndexPath(const QVariantList &);
    Q_SLOT void checkoutBranch();
    Q_SLOT void deleteBranch();
    Q_SLOT void mergeBranch();
    Q_SLOT void rebaseBranch();
    Q_SLOT void fetchBranch();
    Q_SLOT void pullBranch();
    Q_SLOT void pushBranch();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GitLogPage::Actions)

#endif /* GITLOGPAGE_H_ */
