/*
 * GitCommitInfoPage.h
 *
 *  Created on: Aug 24, 2015
 *      Author: lingnan
 */

#ifndef GITCOMMITINFOPAGE_H_
#define GITCOMMITINFOPAGE_H_

#include <bb/cascades/ListItemProvider>
#include <bb/cascades/DataModel>
#include <bb/cascades/Container>
#include <libqgit2/qgitcommit.h>
#include <libqgit2/qgitdiff.h>
#include <PushablePage.h>

namespace bb {
    namespace cascades {
        class Label;
        class ListView;
        class ActionItem;
    }
}

class GitRepoPage;

class GitCommitInfoPage : public PushablePage
{
    Q_OBJECT
public:
    GitCommitInfoPage(GitRepoPage *);
    void setCommit(const LibQGit2::Commit &commit);
    void resetCommit();
    enum Action {
        Checkout = 1u << 0,
    };
    Q_DECLARE_FLAGS(Actions, Action)
    void setActions(Actions=Actions());
    Q_SLOT void checkout();
    Q_SLOT void onTranslatorChanged();
Q_SIGNALS:
    void translatorChanged();
private:
    GitRepoPage *_repoPage;
    struct Diff {
        LibQGit2::Diff diff;
        QString parentOId;
        Diff(const LibQGit2::Diff &d, const QString &o=QString()):
            diff(d), parentOId(o) {}
    };
    class DiffDataModel : public bb::cascades::DataModel {
    public:
        int childCount(const QVariantList &);
        bool hasChildren(const QVariantList &);
        QString itemType(const QVariantList &);
        QVariant data(const QVariantList &);
        void setCommit(const LibQGit2::Commit &, const QList<Diff> &);
        const LibQGit2::Commit &commit() const { return _commit; }
        const QList<Diff> &diffs() const { return _diffs; }
        void refresh();
        void clear();
    private:
        LibQGit2::Commit _commit;
        QList<Diff> _diffs;
    } _dataModel;
    struct LabelSegment : public bb::cascades::Container {
        bb::cascades::Label *label;
        LabelSegment();
    };
    class DiffItemProvider : public bb::cascades::ListItemProvider {
    public:
        DiffItemProvider(GitCommitInfoPage *page): _page(page) {}
        bb::cascades::VisualNode *createItem(bb::cascades::ListView *list, const QString &type);
        void updateItem(bb::cascades::ListView *list, bb::cascades::VisualNode *listItem, const QString &type,
            const QVariantList &indexPath, const QVariant &data);
    private:
        GitCommitInfoPage *_page;
    } _itemProvider;
    bb::cascades::ListView *_listView;
    bb::cascades::ActionItem *_checkoutAction;

    Q_SLOT void showDiffIndexPath(const QVariantList &);
    Q_SLOT void showDiffSelection();
};

#endif /* GITCOMMITINFOPAGE_H_ */
