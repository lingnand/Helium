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
#include <libqgit2/qgitstatuslist.h>
#include <GitWorker.h>
#include <PushablePage.h>

namespace bb {
    namespace cascades {
        class ActionItem;
        class Label;
        class Control;
        class ListView;
        class VisualNode;
    }
}

class Project;
class Segment;
class GitDiffPage;
class GitLogPage;
class GitCommitPage;

class GitRepoPage : public PushablePage
{
    Q_OBJECT
public:
    GitRepoPage(Project *);
    virtual ~GitRepoPage();
    Q_SLOT void reload(); // refresh the view
    Q_SLOT void addAll(const QList<QString> &pathspecs=QList<QString>());
    Q_SLOT void resetAll(const QList<QString> &pathspecs=QList<QString>());
    Q_SLOT bool commit(const QString &);
    Q_SLOT void onPagePopped(bb::cascades::Page *);
    void onTranslatorChanged(bool reload=true);
    bb::cascades::ListView *statusListView() const;
    Q_SLOT void selectAllOnIndex();
    Q_SLOT void selectAllOnWorkdir();
Q_SIGNALS:
    void translatorChanged();
    void workerFetchStatusList();
    void workerAddPaths(const QList<QString> &);
    void workerResetPaths(const QList<QString> &);
private:
    Project *_project;
    // UIs that apply when there is no repo
    bb::cascades::ActionItem *_initAction, *_cloneAction, *_reloadAction;
    bb::cascades::Label *_noRepoLabel;
    bb::cascades::Control *_noRepoContent;
    // UIs for an existing repo
    bb::cascades::ActionItem *_commitAction, *_branchesAction, *_logAction;
    bb::cascades::ActionItem *_addAllAction, *_resetAllAction;
    class StatusDataModel : public bb::cascades::DataModel {
    public:
        int childCount(const QVariantList &);
        bool hasChildren(const QVariantList &);
        QString itemType(const QVariantList &);
        QVariant data(const QVariantList &);
        size_t validDiffDeltasToAdd() const;
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
    bb::cascades::ActionItem *_multiAddAction, *_multiResetAction;
    // lazily instantiated
    GitDiffPage *_diffPage;
    bb::cascades::ActionItem *_diffAddAction;
    bb::cascades::ActionItem *_diffResetAction;
    GitLogPage *_logPage;
    GitCommitPage *_commitPage;

    GitWorker _worker;

    bb::cascades::Control *_repoContent;
    QThread _workerThread;

    Q_SLOT void init();
    Q_SLOT void clone();
    Q_SLOT void branches();
    Q_SLOT void log();
    GitDiffPage *diffPage();
    bb::cascades::ActionItem *diffAddAction();
    Q_SLOT void reloadDiffAddActionTitle();
    Q_SLOT void diffPageAddFile();
    bb::cascades::ActionItem *diffResetAction();
    Q_SLOT void reloadDiffResetActionTitle();
    Q_SLOT void diffPageResetFile();
    Q_SLOT void showCommitPage();
    Q_SLOT void showDiffSelection();
    Q_SLOT void showDiffIndexPath(const QVariantList &);
    Q_SLOT void addSelections();
    Q_SLOT void resetSelections();
    void hideAllActions();

    // multi selection
    void selectAllChildren(const QVariantList &);
    Q_SLOT void reloadMultiSelectActionsEnabled();

    void lockRepoContent();
    Q_SLOT void handleStatusList(const LibQGit2::StatusList &);
    Q_SLOT void onProjectPathChanged();
};

#endif /* GITREPOPAGE_H_ */
