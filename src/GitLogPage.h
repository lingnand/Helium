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

class GitRepoPage;

class GitLogPage : public PushablePage
{
    Q_OBJECT
public:
    GitLogPage(GitRepoPage *);
    // this should always be a valid reference
    void setReference(const LibQGit2::Reference &);
    void resetReference();
    void onTranslatorChanged();
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
        bb::cascades::VisualNode *createItem(bb::cascades::ListView *list, const QString &type);
        void updateItem(bb::cascades::ListView *list, bb::cascades::VisualNode *listItem, const QString &type,
            const QVariantList &indexPath, const QVariant &data);
    } _commitItemProvider;
    Q_SLOT void showCommitIndexPath(const QVariantList &);
};

#endif /* GITLOGPAGE_H_ */
