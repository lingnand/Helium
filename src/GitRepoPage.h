/*
 * GitRepoPage.h
 *
 *  Created on: Aug 8, 2015
 *      Author: lingnan
 */

#ifndef GITREPOPAGE_H_
#define GITREPOPAGE_H_

#include <bb/cascades/ListItemProvider>
#include <bb/cascades/DataModel>
#include <bb/cascades/ActionBarPlacement>
#include <bb/cascades/ProgressIndicatorState>
#include <bb/system/SystemUiResult>
#include <libqgit2/qgitdiffdelta.h>
#include <libqgit2/qgitpatch.h>
#include <libqgit2/qgitrepository.h>
#include <GitDiffPage.h>
#include <GitCommitInfoPage.h>
#include <GitLogPage.h>
#include <GitRemoteInfoPage.h>
#include <PushablePage.h>

namespace bb {
    namespace cascades {
        class ActionItem;
        class Label;
        class Control;
        class Container;
        class ListView;
        class VisualNode;
    }
}

namespace LibQGit2 {
    class Repository;
    class Remote;
}

class Project;
class Segment;
class GitCommitPage;
class GitBranchPage;
class GitSettingsPage;
class AutoHideProgressIndicator;

class GitRepoPage : public PushablePage
{
    Q_OBJECT
public:
    GitRepoPage();
    LibQGit2::Repository *repo();
    Q_SLOT void setProject(Project *);
    Q_SLOT void resetProject(); // detach from a project
    Q_SLOT void reload(); // refresh the view
    Q_SLOT void addPaths(const QList<QString> &pathspecs=QList<QString>());
    Q_SLOT void addAll();
    Q_SLOT void resetPaths(const QList<QString> &pathspecs=QList<QString>());
    Q_SLOT void resetMixed();
    Q_SLOT void safeResetHard();
    Q_SLOT void commit(const QString &);
    Q_SLOT void checkoutCommit(const LibQGit2::Object &);
    Q_SLOT void checkoutBranch(const LibQGit2::Reference &);
    Q_SLOT void merge(const LibQGit2::Reference &);
    Q_SLOT void rebase(const LibQGit2::Reference &upstream);
    Q_SLOT void deleteBranch(const LibQGit2::Reference &);
    Q_SLOT void safeDeleteBranch(const LibQGit2::Reference &);
    Q_SLOT void createBranch(const QString &);
    Q_SLOT void fetch(LibQGit2::Remote *, const LibQGit2::Reference &branch);
    Q_SLOT void fetchBaseAndPrune(LibQGit2::Remote *);
    Q_SLOT void pull(LibQGit2::Remote *, const LibQGit2::Reference &branch);
    Q_SLOT void push(LibQGit2::Remote *, const QString &branch);
    Q_SLOT void safePush(LibQGit2::Remote *, const QString &branch);
    Q_SLOT void createRemote(const QString &name, const QString &url);
    Q_SLOT void setRemoteUrl(LibQGit2::Remote *, const QString &url);
    Q_SLOT void clone(const QString &url);
    Q_SLOT void pushSettingsPage();
    Q_SLOT void pushDiffPage(const LibQGit2::Patch &patch, GitDiffPage::Actions=GitDiffPage::Actions());
    Q_SLOT void pushLogPage(const LibQGit2::Reference &ref, LibQGit2::Remote *remote=NULL, GitLogPage::Actions=GitLogPage::Actions());
    Q_SLOT void pushCommitInfoPage(const LibQGit2::Commit &commit, GitCommitInfoPage::Actions=GitCommitInfoPage::Actions());
    Q_SLOT void pushCommitPage(const QString &hintMessage=QString());
    Q_SLOT void pushRemoteInfoPage(GitRemoteInfoPage::Mode mode, LibQGit2::Remote *remote=NULL);
    Q_SLOT void onPagePopped(bb::cascades::Page *);
    Q_SLOT void onTranslatorChanged(bool reload=true);
    Q_SLOT void selectAllOnIndex();
    Q_SLOT void selectAllOnWorkdir();
    enum StatusDiffType { HeadToIndex, IndexToWorkdir };
    struct StatusDiffDelta {
        StatusDiffType type;
        LibQGit2::DiffDelta delta;
        StatusDiffDelta(StatusDiffType t=HeadToIndex, const LibQGit2::DiffDelta &d=LibQGit2::DiffDelta()):
            type(t), delta(d) {}
    };
    // git worker export
    bool inProgress();
Q_SIGNALS:
    void translatorChanged();
    void inProgressChanged(bool);
    void progressChanged(float, bb::cascades::ProgressIndicatorState::Type);
    void progressDismissed();
    void workerFetchStatusList();
    void workerAddPaths(const QList<QString> &);
    void workerResetPaths(const QList<QString> &);
    void workerRebase(const LibQGit2::Reference &);
    void workerRebaseNext();
    void workerRebaseAbort();
    void workerCommit(const QString &);
    void workerReset(LibQGit2::Repository::ResetType);
    void workerCheckoutCommit(const LibQGit2::Object &);
    void workerCheckoutBranch(const LibQGit2::Reference &);
    void workerMerge(const LibQGit2::Reference &);
    void workerCleanupState();
    void workerDeleteBranch(LibQGit2::Reference);
    void workerCreateBranch(const QString &);
    void workerFetch(LibQGit2::Remote *, const LibQGit2::Reference &branch);
    void workerFetchBaseAndPrune(LibQGit2::Remote *);
    void workerPull(LibQGit2::Remote *, const LibQGit2::Reference &branch);
    void workerPush(LibQGit2::Remote *, const QString &branch);
    void workerCreateRemote(const QString &name, const QString &url, const LibQGit2::Credentials &);
    void workerClone(const QString &url, const QString &path, const LibQGit2::Credentials &);
private:
    Project *_project;
    bb::cascades::Container *_contentHolder;
    // UIs that apply when there is no repo
    bb::cascades::ActionItem *_initAction, *_cloneAction, *_reloadAction, *_settingsAction;
    bb::cascades::Label *_noRepoLabel;
    bb::cascades::Control *_noRepoContent;
    // UIs for an existing repo
    bb::cascades::ActionItem *_commitAction, *_branchesAction, *_logAction;
    bb::cascades::ActionItem *_addAllAction, *_resetMixedAction, *_resetHardAction;
    class StatusDataModel : public bb::cascades::DataModel {
    public:
        int childCount(const QVariantList &);
        bool hasChildren(const QVariantList &);
        QString itemType(const QVariantList &);
        QVariant data(const QVariantList &);
        bool hasValidDiffDeltasInWorkdir() const;
        bool hasValidDiffDeltasInIndex() const;
        const LibQGit2::StatusList &statusList() const { return _statusList; }
        void setStatusList(const LibQGit2::StatusList &);
        // reset, but keep the cached views untouched (stale)
        void resetStatusList();
    private:
        LibQGit2::StatusList _statusList;
    } _statusDataModel;
    class StatusItemProvider : public bb::cascades::ListItemProvider {
    public:
        StatusItemProvider(GitRepoPage *page): _gitRepoPage(page) {}
        bb::cascades::VisualNode *createItem(bb::cascades::ListView *list, const QString &type);
        void updateItem(bb::cascades::ListView *list, bb::cascades::VisualNode *listItem, const QString &type,
            const QVariantList &indexPath, const QVariant &data);
    private:
        GitRepoPage *_gitRepoPage;
    } _statusItemProvider;
    bb::cascades::ListView *_statusListView;
    bb::cascades::Control *_repoContent;
    AutoHideProgressIndicator *_progressIndicator;
    bb::cascades::ActionItem *_multiAddAction, *_multiResetAction;
    bb::cascades::ActionItem *_rebaseNextAction, *_rebaseAbortAction;
    bb::cascades::ActionItem *_mergeAbortAction;
    // lazily instantiated
    GitDiffPage *_diffPage;
    GitLogPage *_logPage;
    GitCommitPage *_commitPage;
    GitCommitInfoPage *_commitInfoPage;
    GitBranchPage *_branchPage;
    GitRemoteInfoPage *_remoteInfoPage;
    GitSettingsPage *_settingsPage;
    LibQGit2::Reference _tempTarget;
    LibQGit2::Remote *_tempRemote;
    QString _tempBranch;

    Q_SLOT void init();
    Q_SLOT void pushBranchPage();
    Q_SLOT void pushLogPage();
    Q_SLOT void safePushRemoteInfoPageForClone();
    Q_SLOT void showDiffSelection();
    Q_SLOT void showDiffIndexPath(const QVariantList &);
    Q_SLOT void addSelections();
    Q_SLOT void resetSelections();
    void hideAllActions();

    // multi selection
    void selectAllChildren(const QVariantList &);
    Q_SLOT void reloadMultiSelectActionsEnabled();

    Q_SLOT void reloadContent();
    Q_SLOT void handleStatusList(const LibQGit2::StatusList &);
    Q_SLOT void onGitWorkerInProgressChanged(bool);
    Q_SLOT void onProjectPathChanged();

    Q_SLOT void onSelectAllOnIndexTriggered();
    Q_SLOT void onSelectAllOnWorkdirTriggered();

    Q_SLOT void onCloneDialogFinished(bb::system::SystemUiResult::Type);
    Q_SLOT void onResetHardDialogFinished(bb::system::SystemUiResult::Type);
    Q_SLOT void onDeleteBranchDialogFinished(bb::system::SystemUiResult::Type);
    Q_SLOT void onPushDialogFinished(bb::system::SystemUiResult::Type);

    Q_SLOT void mergeAbort();

    Q_SLOT void rebaseNext();
    Q_SLOT void rebaseAbort();
};

Q_DECLARE_METATYPE(GitRepoPage::StatusDiffDelta)

#endif /* GITREPOPAGE_H_ */
