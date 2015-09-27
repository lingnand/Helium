/*
 * GitBranchPage.h
 *
 *  Created on: Aug 28, 2015
 *      Author: lingnan
 */

#ifndef GITBRANCHPAGE_H_
#define GITBRANCHPAGE_H_

#include <bb/cascades/ListItemProvider>
#include <bb/cascades/DataModel>
#include <bb/cascades/Header>
#include <bb/cascades/ContextMenuVisualState>
#include <bb/system/SystemUiResult>
#include <libqgit2/qgitref.h>
#include <PushablePage.h>

namespace bb {
    namespace cascades {
        class ListView;
        class AcitonItem;
    }
}

namespace LibQGit2 {
    class Repository;
    class Remote;
}

class GitRepoPage;
class AutoHideProgressIndicator;

class GitBranchPage : public PushablePage
{
    Q_OBJECT
public:
    GitBranchPage(GitRepoPage *);
    Q_SLOT void reload();
    Q_SLOT void reset();
    void connectToRepoPage();
    void disconnectFromRepoPage();
    Q_SLOT void onTranslatorChanged(bool reload=true);
Q_SIGNALS:
    void translatorChanged();
private:
    GitRepoPage *_repoPage;
    bb::cascades::ActionItem *_addBranchAction, *_addRemoteAction, *_reloadAction;
    class BranchDataModel : public bb::cascades::DataModel {
    public:
        BranchDataModel(GitBranchPage *page, LibQGit2::Repository *repo):
            _page(page), _repo(repo) {}
        int childCount(const QVariantList &);
        bool hasChildren(const QVariantList &);
        QString itemType(const QVariantList &);
        QVariant data(const QVariantList &);
        void reload();
        void clear();
    private:
        GitBranchPage *_page;
        LibQGit2::Repository *_repo;
        QList<LibQGit2::Reference> _localBranches;
        struct RemoteInfo {
            LibQGit2::Remote *remote;
            QList<LibQGit2::Reference> branches;
            RemoteInfo(LibQGit2::Remote *r=NULL): remote(r) {}
        };
        QList<RemoteInfo> _remoteInfos;
    } _dataModel;
    class BranchItemProvider : public bb::cascades::ListItemProvider {
    public:
        BranchItemProvider(GitBranchPage *page): _page(page) {}
        bb::cascades::VisualNode *createItem(bb::cascades::ListView *list, const QString &type);
        void updateItem(bb::cascades::ListView *list, bb::cascades::VisualNode *listItem, const QString &type,
            const QVariantList &indexPath, const QVariant &data);
    private:
        GitBranchPage *_page;
    } _itemProvider;
    bb::cascades::ListView *_branchList;
    AutoHideProgressIndicator *_progressIndicator;
    LibQGit2::Remote *_tempRemote;

    Q_SLOT void onBranchListTriggered(const QVariantList &);
    Q_SLOT void showBranchLogSelection();
    Q_SLOT void onRemoteHeaderContextMenuVisualStateChanged(bb::cascades::ContextMenuVisualState::Type);
    Q_SLOT void showInfoRemoteSelection();
    void showRemoteInfo(const QVariantList &);
    Q_SLOT void refreshRemoteSelection();
    Q_SLOT void pushToBranchRemoteSelection();
    Q_SLOT void onPushToBranchPromptFinished(bb::system::SystemUiResult::Type, const QString &);
    Q_SLOT void deleteBranchSelection();
    Q_SLOT void checkoutBranchSelection();
    Q_SLOT void mergeBranchSelection();
    Q_SLOT void rebaseBranchSelection();
    Q_SLOT void fetchBranchSelection();
    Q_SLOT void pullBranchSelection();
    Q_SLOT void pushBranchSelection();
    Q_SLOT void addBranch();
    Q_SLOT void addRemote();
    Q_SLOT void onAddBranchPromptFinished(bb::system::SystemUiResult::Type, const QString &);
    Q_SLOT void onGitRepoPageInProgressChanged(bool);
};

#endif /* GITBRANCHPAGE_H_ */
