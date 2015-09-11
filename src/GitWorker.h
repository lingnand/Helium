/*
 * GitWorker.h
 *
 *  Created on: Aug 23, 2015
 *      Author: lingnan
 */

#ifndef GITWORKER_H_
#define GITWORKER_H_

#include <bb/cascades/ProgressIndicatorState>
#include <libqgit2/qgitstatuslist.h>
#include <libqgit2/qgitrebase.h>
#include <libqgit2/qgitref.h>
#include <libqgit2/qgitrepository.h>
#include <Progress.h>

namespace LibQGit2 {
    class Repository;
}

class GitWorker : public QObject
{
    Q_OBJECT
public:
    GitWorker(LibQGit2::Repository *);
    Q_SLOT void fetchStatusList(Progress progress=Progress());
    Q_SLOT void addPaths(const QList<QString> &, Progress progress=Progress());
    Q_SLOT void resetPaths(const QList<QString> &, Progress progress=Progress());
    Q_SLOT void reset(LibQGit2::Repository::ResetType, Progress progress=Progress());
    Q_SLOT void rebase(const LibQGit2::Reference &, Progress progress=Progress());
    Q_SLOT void rebaseNext(Progress progress=Progress());
    Q_SLOT void rebaseAbort(Progress progress=Progress());
    Q_SLOT void commit(const QString &, Progress progress=Progress());
    Q_SLOT void checkoutCommit(const LibQGit2::Object &, Progress progress=Progress());
    Q_SLOT void checkoutBranch(const LibQGit2::Reference &, Progress progress=Progress());
    Q_SLOT void merge(const LibQGit2::Reference &, Progress progress=Progress());
    Q_SLOT void cleanupState(Progress progress=Progress());
    Q_SLOT void setAuthorName(const QString &);
    Q_SLOT void setAuthorEmail(const QString &);
Q_SIGNALS:
    void progressChanged(float progress, bb::cascades::ProgressIndicatorState::Type state=bb::cascades::ProgressIndicatorState::Progress);
    void statusListFetched(const LibQGit2::StatusList &);
    void progressFinished();
    void progressDismissed();
    void pushCommitPage(const QString &);
private:
    LibQGit2::Repository *_repo;
    LibQGit2::Rebase _rebase;
    QString _name, _email;  // for signature

    LibQGit2::Rebase &rebaseObj();

    void _fetchStatusList(Progress progress);
};

#endif /* GITWORKER_H_ */
