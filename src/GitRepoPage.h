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

class GitRepoPage : public PushablePage
{
    Q_OBJECT
public:
    GitRepoPage(Project *);
    Q_SLOT void reload(); // refresh the view
    Q_SLOT void onTranslatorChanged();
    void addPaths(const QList<QString> &);
    void resetPaths(const QList<QString> &);
    Q_SLOT void onPopTransitionEnded(bb::cascades::Page *);
Q_SIGNALS:
    void translatorChanged();
private:
    Project *_project;
    // UIs that apply when there is no repo
    bb::cascades::ActionItem *_initAction, *_cloneAction, *_reloadAction;
    bb::cascades::Label *_noRepoLabel;
    bb::cascades::Control *_noRepoContent;
    // UIs for an existing repo
    bb::cascades::ActionItem *_commitAction, *_branchesAction, *_logAction;
    class StatusDataModel : public bb::cascades::DataModel {
    public:
        int childCount(const QVariantList &);
        bool hasChildren(const QVariantList &);
        QString itemType(const QVariantList &);
        QVariant data(const QVariantList &);
        void setStatusList(const LibQGit2::StatusList &);
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
    bb::cascades::ListView *_repoContent;
    bb::cascades::ActionItem *_multiAddAction, *_multiResetAction;
    GitDiffPage *_diffPage;

    Q_SLOT void init();
    Q_SLOT void clone();
    Q_SLOT void commit();
    Q_SLOT void branches();
    Q_SLOT void log();
    Q_SLOT void diffSelection();
    Q_SLOT void diffIndexPath(const QVariantList &);
    Q_SLOT void addSelections();
    Q_SLOT void resetSelections();
    void hideAllActions();
};

#endif /* GITREPOPAGE_H_ */
