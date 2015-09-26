/*
 * GitWorker.h
 *
 *  Created on: Aug 23, 2015
 *      Author: lingnan
 */

#ifndef GITWORKER_H_
#define GITWORKER_H_

#include <QMutex>
#include <bb/cascades/ProgressIndicatorState>
#include <libqgit2/qgitstatuslist.h>
#include <libqgit2/qgitrebase.h>
#include <libqgit2/qgitref.h>
#include <libqgit2/qgitrepository.h>
#include <Progress.h>

namespace LibQGit2 {
    class Repository;
    class Remote;
}

class GitWorker : public QObject
{
    Q_OBJECT
public:
    GitWorker(LibQGit2::Repository *);
    bool inProgress(); // whether an operation is running with the current worker
    Q_SLOT void fetchStatusList(Progress=Progress());
    Q_SLOT void addPaths(const QList<QString> &, Progress=Progress());
    Q_SLOT void resetPaths(const QList<QString> &, Progress=Progress());
    Q_SLOT void reset(LibQGit2::Repository::ResetType, Progress=Progress());
    Q_SLOT void rebase(const LibQGit2::Reference &, Progress=Progress());
    Q_SLOT void rebaseNext(Progress=Progress());
    Q_SLOT void rebaseAbort(Progress=Progress());
    Q_SLOT void commit(const QString &, Progress=Progress());
    Q_SLOT void checkoutCommit(const LibQGit2::Object &, Progress=Progress());
    Q_SLOT void checkoutBranch(const LibQGit2::Reference &, Progress=Progress());
    Q_SLOT void merge(const LibQGit2::Reference &, Progress=Progress());
    Q_SLOT void cleanupState(Progress=Progress());
    Q_SLOT void deleteBranch(LibQGit2::Reference, Progress=Progress());
    Q_SLOT void createBranch(const QString &name, Progress=Progress());
    Q_SLOT void fetch(LibQGit2::Remote *, const LibQGit2::Reference &branch, Progress=Progress());
    Q_SLOT void fetchBaseAndPrune(LibQGit2::Remote *, Progress=Progress());
    Q_SLOT void pull(LibQGit2::Remote *, const LibQGit2::Reference &branch, Progress=Progress());
    Q_SLOT void push(LibQGit2::Remote *, const QString &branch, Progress=Progress());
    Q_SLOT void createRemote(const QString &name, const QString &url, const LibQGit2::Credentials &, Progress=Progress());
    Q_SLOT void clone(const QString &url, const QString &path, const LibQGit2::Credentials &credentials, Progress=Progress());
    Q_SLOT void setAuthorName(const QString &);
    Q_SLOT void setAuthorEmail(const QString &);
Q_SIGNALS:
    void inProgressChanged(bool);
    void progressChanged(float progress, bb::cascades::ProgressIndicatorState::Type state=bb::cascades::ProgressIndicatorState::Progress);
    void progressDismissed();
    void statusListFetched(const LibQGit2::StatusList &);
    void pushCommitPage(const QString &);
private:
    LibQGit2::Repository *_repo;
    QMutex _inProgressMut;
    bool _inProgress;

    LibQGit2::Rebase _rebase;
    QString _name, _email;  // for signature

    // for remote
    float *_currentProgress, _progressStart, _progressInc;
    Q_SLOT void onRemoteTransferProgress(int);

    void setInProgress(bool);
    LibQGit2::Rebase &rebaseObj();

    void _fetchStatusList(Progress);
    void _merge(const LibQGit2::Reference &, Progress);
    bool _fetch(LibQGit2::Remote *, const LibQGit2::Reference &branch, Progress);
    void _fetchBaseAndPrune(LibQGit2::Remote *, Progress);
};

#endif /* GITWORKER_H_ */
