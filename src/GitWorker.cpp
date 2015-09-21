/*
 * GitWorker.cpp
 *
 *  Created on: Aug 23, 2015
 *      Author: lingnan
 */

#include <QMutexLocker>
#include <QDebug>
#include <libqgit2/qgitexception.h>
#include <libqgit2/qgitstatusoptions.h>
#include <libqgit2/qgitrepository.h>
#include <libqgit2/qgitremote.h>
#include <GitWorker.h>
#include <Utility.h>

using namespace bb::cascades;

GitWorker::GitWorker(LibQGit2::Repository *repo):
    _repo(repo), _inProgress(false),
    _currentProgress(NULL), _progressStart(0), _progressInc(0)
{
}

bool GitWorker::inProgress()
{
    QMutexLocker lock(&_inProgressMut);
    return _inProgress;
}

void GitWorker::setInProgress(bool inProgress)
{
    QMutexLocker lock(&_inProgressMut);
    if (inProgress != _inProgress) {
        _inProgress = inProgress;
        emit inProgressChanged(_inProgress);
    }
}

void GitWorker::setAuthorName(const QString &name)
{
    _name = name;
}

void GitWorker::setAuthorEmail(const QString &email)
{
    _email = email;
}

void GitWorker::_fetchStatusList(Progress progress)
{
    emit progressChanged(progress.current+=(progress.cap-progress.current)/2);
    try {
        const LibQGit2::StatusList statusList = _repo->status(
            LibQGit2::StatusOptions(LibQGit2::StatusOptions::ShowIndexAndWorkdir,
                    LibQGit2::StatusOptions::IncludeUntracked |
    //                        LibQGit2::StatusOptions::ExcludeSubmodules |
                    LibQGit2::StatusOptions::RenamesHeadToIndex |
                    LibQGit2::StatusOptions::RenamesIndexToWorkdir));
        emit progressChanged(progress.cap);
        emit statusListFetched(statusList);
    } catch (const LibQGit2::Exception &e) {
        qDebug() << "::::LIBQGIT2 ERROR when fetching status::::" << e.what();
        emit progressChanged(progress.current, ProgressIndicatorState::Error);
        Utility::toast(e.what(), tr("OK"), this, SIGNAL(progressDismissed()));
    }
}

LibQGit2::Rebase &GitWorker::rebaseObj()
{
    if (_rebase.isNull())
        _rebase = _repo->openRebase(
            LibQGit2::RebaseOptions(LibQGit2::CheckoutOptions(LibQGit2::CheckoutOptions::Safe)));
    return _rebase;
}

void GitWorker::fetchStatusList(Progress progress)
{
    setInProgress(true);
    _fetchStatusList(progress);
    setInProgress(false);
}

void GitWorker::addPaths(const QList<QString> &paths, Progress progress)
{
    setInProgress(true);
    float initInc = (progress.cap-progress.current)/8;
    emit progressChanged(progress.current+=initInc);
    try {
        _repo->index().addAll(paths);
        emit progressChanged(progress.current+=initInc*2);
        _fetchStatusList(progress);
    } catch (const LibQGit2::Exception &e) {
        qDebug() << "::::LIBQGIT2 ERROR when adding paths::::" << e.what();
        emit progressChanged(progress.current, ProgressIndicatorState::Error);
        Utility::toast(e.what(), tr("OK"), this, SIGNAL(progressDismissed()));
    }
    setInProgress(false);
}

void GitWorker::resetPaths(const QList<QString> &paths, Progress progress)
{
    setInProgress(true);
    float initInc = (progress.cap-progress.current)/8;
    emit progressChanged(progress.current+=initInc);
    try {
        _repo->index().removeAll(paths);
        emit progressChanged(progress.current+=initInc*2);
        _fetchStatusList(progress);
    } catch (const LibQGit2::Exception &e) {
        qDebug() << "::::LIBQGIT2 ERROR when reseting paths::::" << e.what();
        emit progressChanged(progress.current, ProgressIndicatorState::Error);
        Utility::toast(e.what(), tr("OK"), this, SIGNAL(progressDismissed()));
    }
    setInProgress(false);
}

void GitWorker::reset(LibQGit2::Repository::ResetType type, Progress progress)
{
    setInProgress(true);
    float initInc = (progress.cap-progress.current)/8;
    emit progressChanged(progress.current+=initInc);
    try {
        _repo->reset(_repo->head().peelToCommit(), type);
        emit progressChanged(progress.current+=initInc*2);
        _fetchStatusList(progress);
    } catch (const LibQGit2::Exception &e) {
        qDebug() << "::::LIBQGIT2 ERROR when reseting mixed::::" << e.what();
        emit progressChanged(progress.current, ProgressIndicatorState::Error);
        Utility::toast(e.what(), tr("OK"), this, SIGNAL(progressDismissed()));
    }
    setInProgress(false);
}

void GitWorker::rebase(const LibQGit2::Reference &upstream, Progress progress)
{
    setInProgress(true);
    float initInc = (progress.cap-progress.current)/8;
    emit progressChanged(progress.current+=initInc);
    try {
        _rebase = _repo->rebase(_repo->head(), upstream, upstream,
                LibQGit2::RebaseOptions(LibQGit2::CheckoutOptions(LibQGit2::CheckoutOptions::Safe)));
        emit progressChanged(progress.current+=initInc*2);
        rebaseNext(progress);
    } catch (const LibQGit2::Exception &e) {
        qDebug() << "::::LIBQGIT2 ERROR when creating rebase::::" << e.what();
        emit progressChanged(progress.current, ProgressIndicatorState::Error);
        Utility::toast(e.what(), tr("OK"), this, SIGNAL(progressDismissed()));
    }
    setInProgress(false);
}

void GitWorker::rebaseNext(Progress progress)
{
    setInProgress(true);
    LibQGit2::Rebase &rebase = rebaseObj();
    LibQGit2::Signature sig = LibQGit2::Signature(_name, _email);
    float inc = (progress.cap-progress.current)/(rebase.operationCount()*1.5+2);
    emit progressChanged(progress.current+=inc*2);
    try {
        while (rebase.next()) {
            rebase.commit(sig);
            emit progressChanged(progress.current+=inc);
        }
        rebase.finish();
        // reset rebase object
        rebase = LibQGit2::Rebase();
    } catch (const LibQGit2::Exception &e) {
        qDebug() << "::::LIBQGIT2 ERROR when rebasing next::::" << e.what();
        emit progressChanged(progress.current, ProgressIndicatorState::Error);
        Utility::toast(e.what(), tr("OK"), this, SIGNAL(progressDismissed()));
    }
    _fetchStatusList(progress);
    setInProgress(false);
}

void GitWorker::rebaseAbort(Progress progress)
{
    setInProgress(true);
    LibQGit2::Rebase &rebase = rebaseObj();
    LibQGit2::Signature sig = LibQGit2::Signature(_name, _email);
    float initInc = (progress.cap-progress.current)/8;
    emit progressChanged(progress.current+=initInc);
    try {
        rebase.abort(sig);
        emit progressChanged(progress.current+=initInc*2);
        _fetchStatusList(progress);
    } catch (const LibQGit2::Exception &e) {
        qDebug() << "::::LIBQGIT2 ERROR when aborting rebase::::" << e.what();
        emit progressChanged(progress.current, ProgressIndicatorState::Error);
        Utility::toast(e.what(), tr("OK"), this, SIGNAL(progressDismissed()));
    }
    setInProgress(false);
}

void GitWorker::commit(const QString &message, Progress progress)
{
    setInProgress(true);
    float initInc = (progress.cap-progress.current)/8;
    emit progressChanged(progress.current+=initInc);
    try {
        QList<LibQGit2::Commit> parents;
        if (!_repo->isHeadUnborn()) {
            parents.append(_repo->head().peelToCommit());
        }
        LibQGit2::Signature sig = LibQGit2::Signature(_name, _email);
        LibQGit2::OId oid = _repo->createCommit(_repo->lookupTree(_repo->index().createTree()),
                parents, sig, sig, message, "HEAD");
        _repo->index().write();
        emit progressChanged(progress.current+=initInc*2);
        _repo->cleanupState();
        emit progressChanged(progress.current+=initInc);
        Utility::toast(tr("Commited %1").arg(QString(oid.nformat(7))));
        _fetchStatusList(progress);
    } catch (const LibQGit2::Exception &e) {
        qDebug() << "::::LIBQGIT2 ERROR when committing::::" << e.what();
        emit progressChanged(progress.current, ProgressIndicatorState::Error);
        Utility::toast(e.what(), tr("OK"), this, SIGNAL(progressDismissed()));
    }
    setInProgress(false);
}

void GitWorker::checkoutCommit(const LibQGit2::Object &commit, Progress progress)
{
    setInProgress(true);
    float initInc = (progress.cap-progress.current)/8;
    emit progressChanged(progress.current+=initInc);
    try {
        _repo->checkoutTree(commit,
                LibQGit2::CheckoutOptions(LibQGit2::CheckoutOptions::Safe));
        emit progressChanged(progress.current+=initInc*2);
        const LibQGit2::OId &oid = commit.oid();
        _repo->setHeadDetached(oid);
        emit progressChanged(progress.current+=initInc);
        Utility::toast(tr("HEAD detached at %1").arg(QString(oid.nformat(7))));
        _fetchStatusList(progress);
    } catch (const LibQGit2::Exception &e) {
        qDebug() << "::::LIBQGIT2 ERROR when checking out commit::::" << e.what();
        emit progressChanged(progress.current, ProgressIndicatorState::Error);
        Utility::toast(e.what(), tr("OK"), this, SIGNAL(progressDismissed()));
    }
    setInProgress(false);
}

void GitWorker::checkoutBranch(const LibQGit2::Reference &branch, Progress progress)
{
    setInProgress(true);
    float initInc = (progress.cap-progress.current)/8;
    emit progressChanged(progress.current+=initInc);
    try {
        _repo->checkoutTree(branch.peelToObject(),
                LibQGit2::CheckoutOptions(LibQGit2::CheckoutOptions::Safe));
        emit progressChanged(progress.current+=initInc*2);
        _repo->setHead(branch.name());
        emit progressChanged(progress.current+=initInc);
        Utility::toast(tr("HEAD now on branch %1").arg(branch.branchName()));
        _fetchStatusList(progress);
    } catch (const LibQGit2::Exception &e) {
        qDebug() << "::::LIBQGIT2 ERROR when checking out branch::::" << e.what();
        emit progressChanged(progress.current, ProgressIndicatorState::Error);
        Utility::toast(e.what(), tr("OK"), this, SIGNAL(progressDismissed()));
    }
    setInProgress(false);
}

void GitWorker::merge(const LibQGit2::Reference &theirHead, Progress progress)
{
    setInProgress(true);
    _merge(theirHead, progress);
    setInProgress(false);
}

void GitWorker::_merge(const LibQGit2::Reference &theirHead, Progress progress)
{
    float initInc = (progress.cap-progress.current)/8;
    emit progressChanged(progress.current+=initInc);
    try {
        QList<LibQGit2::Reference> theirHeads;
        theirHeads.append(theirHead);
        LibQGit2::Repository::MergeAnalysisFlags analysis = _repo->mergeAnalysis(theirHeads);
        qDebug() << "OBTAINED merge anlysis result" << analysis;
        if (analysis.testFlag(LibQGit2::Repository::MergeAnalysisUpToDate)) {
            qDebug() << "Up to date merge detected";
            Utility::toast(tr("Merge input is reachable from HEAD: no merge required!"));
            emit progressChanged(progress.cap);
        } else if (analysis.testFlag(LibQGit2::Repository::MergeAnalysisFastforward)) {
            qDebug() << "Fast forward merge detected";
            // checkout the tree of theirHead
            qDebug() << "START checking out of tree";
            LibQGit2::Reference ref = theirHead.resolve();
            _repo->checkoutTree(ref.peelToObject(),
                    LibQGit2::CheckoutOptions(LibQGit2::CheckoutOptions::Safe));
            qDebug() << "FINISHED checking out of tree";
            emit progressChanged(progress.current+=initInc*2);
            // update the current branch to point to what theirHead points to
            LibQGit2::OId target = ref.target();
            _repo->head().resolve().setTarget(target);
            emit progressChanged(progress.current+=initInc);
            Utility::toast(tr("Fast-forwarded to %1").arg(QString(target.nformat(7))));
            _fetchStatusList(progress); // TODO: do we really need to fetch status list?
        } else if (analysis.testFlag(LibQGit2::Repository::MergeAnalysisNormal)) {
            qDebug() << "Normal merge detected";
            qDebug() << "START merging";
            _repo->merge(theirHeads, LibQGit2::MergeOptions(),
                    LibQGit2::CheckoutOptions(LibQGit2::CheckoutOptions::Safe));
            qDebug() << "FINISHED merging";
            emit progressChanged(progress.current+=initInc*2);
            if (_repo->index().hasConflicts()) {
                emit progressChanged(progress.current, ProgressIndicatorState::Error);
                Utility::toast(tr("Conflicts detected: fix them and run \"Commit\""),
                        tr("OK"), this, SIGNAL(progressDismissed()));
            } else
                emit pushCommitPage(tr("Commit message for merging %1")
                        .arg(QString(theirHead.name())));
            _fetchStatusList(progress);
        } else if (analysis.testFlag(LibQGit2::Repository::MergeAnalysisUnborn)) {
            emit progressChanged(progress.current, ProgressIndicatorState::Error);
            Utility::toast(tr("HEAD is unborn and doesn't point to a valid commit: no merge can be performed!"),
                    tr("OK"), this, SIGNAL(progressDismissed()));
        } else {
            emit progressChanged(progress.current, ProgressIndicatorState::Error);
            Utility::toast(tr("No merge is possible!"), tr("OK"),
                    this, SIGNAL(progressDismissed()));
        }
    } catch (const LibQGit2::Exception &e) {
        qDebug() << "::::LIBQGIT2 ERROR when merging branch::::" << e.what();
        emit progressChanged(progress.current, ProgressIndicatorState::Error);
        Utility::toast(e.what(), tr("OK"), this, SIGNAL(progressDismissed()));
    }
}

void GitWorker::cleanupState(Progress progress)
{
    setInProgress(true);
    float initInc = (progress.cap-progress.current)/8;
    emit progressChanged(progress.current+=initInc);
    try {
        _repo->cleanupState();
        emit progressChanged(progress.current+=initInc*2);
        _fetchStatusList(progress);
    } catch (const LibQGit2::Exception &e) {
        qDebug() << "::::LIBQGIT2 ERROR when cleaning up state::::" << e.what();
        emit progressChanged(progress.current, ProgressIndicatorState::Error);
        Utility::toast(e.what(), tr("OK"), this, SIGNAL(progressDismissed()));
    }
    setInProgress(false);
}

void GitWorker::deleteBranch(LibQGit2::Reference branch, Progress progress)
{
    setInProgress(true);
    emit progressChanged(progress.current=(progress.cap+progress.current)/2);
    try {
        branch.branchDelete();
        emit progressChanged(progress.cap);
    } catch (const LibQGit2::Exception &e) {
        qDebug() << "::::LIBQGIT2 ERROR when deleting branch::::" << e.what();
        emit progressChanged(progress.current, ProgressIndicatorState::Error);
        Utility::toast(e.what(), tr("OK"), this, SIGNAL(progressDismissed()));
    }
    setInProgress(false);
}

void GitWorker::createBranch(const QString &name, Progress progress)
{
    setInProgress(true);
    emit progressChanged(progress.current=(progress.cap+progress.current)/2);
    try {
        _repo->createBranch(name);
        emit progressChanged(progress.cap);
    } catch (const LibQGit2::Exception &e) {
        qDebug() << "::::LIBQGIT2 ERROR when creating branch::::" << e.what();
        emit progressChanged(progress.current, ProgressIndicatorState::Error);
        Utility::toast(e.what(), tr("OK"), this, SIGNAL(progressDismissed()));
    }
    setInProgress(false);
}

void GitWorker::fetch(LibQGit2::Remote *remote, const LibQGit2::Reference &branch, Progress progress)
{
    setInProgress(true);
    _fetch(remote, branch, progress);
    setInProgress(false);
}

void GitWorker::fetchBaseAndPrune(LibQGit2::Remote *remote, Progress progress)
{
    setInProgress(true);
    emit progressChanged(progress.current+=(progress.cap-progress.current)/10);
    _currentProgress = &progress.current;
    _progressStart = progress.current;
    _progressInc = (progress.cap-progress.current)*0.7/100;
    conn(remote, SIGNAL(transferProgress(int)),
            this, SLOT(onRemoteTransferProgress(int)));
    try {
        // use the default fetch refspecs
        remote->fetch();
        remote->prune();
        emit progressChanged(progress.cap);
        Utility::toast(tr("Fetch-and-pruned for remote %1").arg(remote->name()));
    } catch (const LibQGit2::Exception &e) {
        qDebug() << "::::LIBQGIT2 ERROR when fetching all::::" << e.what();
        emit progressChanged(progress.current, ProgressIndicatorState::Error);
        Utility::toast(e.what(), tr("OK"), this, SIGNAL(progressDismissed()));
    }
    remote->disconnect(this);
    _currentProgress = NULL;
    _progressStart = _progressInc = 0;
    setInProgress(false);
}

bool GitWorker::_fetch(LibQGit2::Remote *remote, const LibQGit2::Reference &branch, Progress progress)
{
    bool changed = true;
    emit progressChanged(progress.current+=(progress.cap-progress.current)/10);
    _currentProgress = &progress.current;
    _progressStart = progress.current;
    _progressInc = (progress.cap-progress.current)/100;
    conn(remote, SIGNAL(transferProgress(int)),
            this, SLOT(onRemoteTransferProgress(int)));
    try {
        LibQGit2::OId targetBeforeFetch = branch.resolve().target();
        // assuming that this is run on the same thread
        remote->fetch(branch.branchName().split('/').last());
        LibQGit2::OId targetAfterFetch = branch.resolve().target();
        if (targetBeforeFetch == targetAfterFetch) {
            Utility::toast(tr("Already up-to-date"));
            changed = false;
        } else {
            Utility::toast(tr("Fetched till %1").arg(QString(targetAfterFetch.nformat(7))));
        }
        emit progressChanged(progress.cap);
    } catch (const LibQGit2::Exception &e) {
        qDebug() << "::::LIBQGIT2 ERROR when fetching::::" << e.what();
        emit progressChanged(progress.current, ProgressIndicatorState::Error);
        Utility::toast(e.what(), tr("OK"), this, SIGNAL(progressDismissed()));
        changed = false;
    }
    remote->disconnect(this);
    _currentProgress = NULL;
    _progressStart = _progressInc = 0;
    return changed;
}

void GitWorker::pull(LibQGit2::Remote *remote, const LibQGit2::Reference &branch, Progress progress)
{
    setInProgress(true);
    float mid = progress.current+(progress.cap-progress.current)*0.7;
    _fetch(remote, branch, Progress(progress.current, mid));
    _merge(branch, Progress(mid, progress.cap));
    setInProgress(false);
}

void GitWorker::push(LibQGit2::Remote *remote, const QString &branch, Progress progress)
{
    setInProgress(true);
    emit progressChanged(progress.current+=(progress.cap-progress.current)/10);
    _currentProgress = &progress.current;
    _progressStart = progress.current;
    _progressInc = (progress.cap-progress.current)/100;
    conn(remote, SIGNAL(transferProgress(int)),
            this, SLOT(onRemoteTransferProgress(int)));
    try {
        remote->push(branch);
        Utility::toast(tr("Pushed to %1").arg(branch));
        emit progressChanged(progress.cap);
    } catch (const LibQGit2::Exception &e) {
        qDebug() << "::::LIBQGIT2 ERROR when pushing::::" << e.what();
        emit progressChanged(progress.current, ProgressIndicatorState::Error);
        Utility::toast(e.what(), tr("OK"), this, SIGNAL(progressDismissed()));
    }
    remote->disconnect(this);
    _currentProgress = NULL;
    _progressStart = _progressInc = 0;
    setInProgress(false);
}

void GitWorker::onRemoteTransferProgress(int progress)
{
    qDebug() << "REMOTE TRANSFER PROGRESS" << progress;
    if (_currentProgress) {
        *_currentProgress = _progressStart + progress*_progressInc;
        emit progressChanged(*_currentProgress);
    }
}
