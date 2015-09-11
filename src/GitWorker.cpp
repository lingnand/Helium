/*
 * GitWorker.cpp
 *
 *  Created on: Aug 23, 2015
 *      Author: lingnan
 */

#include <QDebug>
#include <libqgit2/qgitexception.h>
#include <libqgit2/qgitstatusoptions.h>
#include <libqgit2/qgitrepository.h>
#include <GitWorker.h>
#include <Utility.h>

using namespace bb::cascades;

GitWorker::GitWorker(LibQGit2::Repository *repo):
    _repo(repo)
{
}

void GitWorker::setAuthorName(const QString &name)
{
    _name = name;
}

void GitWorker::setAuthorEmail(const QString &email)
{
    _email = email;
}

void GitWorker::fetchStatusList(Progress progress)
{
    _fetchStatusList(progress);
    emit progressFinished();
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

void GitWorker::addPaths(const QList<QString> &paths, Progress progress)
{
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
    emit progressFinished();
}

void GitWorker::resetPaths(const QList<QString> &paths, Progress progress)
{
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
    emit progressFinished();
}

void GitWorker::reset(LibQGit2::Repository::ResetType type, Progress progress)
{
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
    emit progressFinished();
}

LibQGit2::Rebase &GitWorker::rebaseObj()
{
    if (_rebase.isNull())
        _rebase = _repo->openRebase(
            LibQGit2::RebaseOptions(LibQGit2::CheckoutOptions(LibQGit2::CheckoutOptions::Safe)));
    return _rebase;
}

void GitWorker::rebase(const LibQGit2::Reference &upstream, Progress progress)
{
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
    emit progressFinished();
}

void GitWorker::rebaseNext(Progress progress)
{
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
    emit progressFinished();
}

void GitWorker::rebaseAbort(Progress progress)
{
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
    emit progressFinished();
}

void GitWorker::commit(const QString &message, Progress progress)
{
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
    emit progressFinished();
}

void GitWorker::checkoutCommit(const LibQGit2::Object &commit, Progress progress)
{
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
    emit progressFinished();
}

void GitWorker::checkoutBranch(const LibQGit2::Reference &branch, Progress progress)
{
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
    emit progressFinished();
}

void GitWorker::merge(const LibQGit2::Reference &theirHead, Progress progress)
{
    float initInc = (progress.cap-progress.current)/8;
    emit progressChanged(progress.current+=initInc);
    try {
        QList<LibQGit2::Reference> theirHeads;
        theirHeads.append(theirHead);
        switch (_repo->mergeAnalysis(theirHeads)) {
            case LibQGit2::Repository::MergeAnalysisNormal:
                _repo->merge(theirHeads, LibQGit2::MergeOptions(),
                        LibQGit2::CheckoutOptions(LibQGit2::CheckoutOptions::Safe));
                emit progressChanged(progress.current+=initInc*2);
                if (_repo->index().hasConflicts()) {
                    emit progressChanged(progress.current, ProgressIndicatorState::Error);
                    Utility::toast(tr("Conflicts detected: fix them and run \"Commit\""),
                            tr("OK"), this, SIGNAL(progressDismissed()));
                } else
                    emit pushCommitPage(tr("Commit message for merging %1")
                            .arg(QString(theirHead.name())));
                _fetchStatusList(progress);
                break;
            case LibQGit2::Repository::MergeAnalysisFastforward:
                // checkout the tree of theirHead
                _repo->checkoutTree(theirHead.peelToObject(),
                        LibQGit2::CheckoutOptions(LibQGit2::CheckoutOptions::Safe));
                emit progressChanged(progress.current+=initInc*2);
                // update the current branch to point to what theirHead points to
                _repo->head().resolve().setTarget(theirHead.target());
                emit progressChanged(progress.current+=initInc);
                Utility::toast(tr("Fast-forwarded to %1").arg(QString(theirHead.target().nformat(7))));
                _fetchStatusList(progress); // TODO: do we really need to fetch status list?
                break;
            case LibQGit2::Repository::MergeAnalysisUpToDate:
                Utility::toast(tr("Merge input is reachable from HEAD: no merge required!"));
                emit progressDismissed();
                break;
            case LibQGit2::Repository::MergeAnalysisUnborn:
                emit progressChanged(progress.current, ProgressIndicatorState::Error);
                Utility::toast(tr("HEAD is unborn and doesn't point to a valid commit: no merge can be performed!"),
                        tr("OK"), this, SIGNAL(progressDismissed()));
                break;
            case LibQGit2::Repository::MergeAnalysisNone:
                emit progressChanged(progress.current, ProgressIndicatorState::Error);
                Utility::toast(tr("No merge is possible!"), tr("OK"),
                        this, SIGNAL(progressDismissed()));
                break;
        }
    } catch (const LibQGit2::Exception &e) {
        qDebug() << "::::LIBQGIT2 ERROR when merging branch::::" << e.what();
        emit progressChanged(progress.current, ProgressIndicatorState::Error);
        Utility::toast(e.what(), tr("OK"), this, SIGNAL(progressDismissed()));
    }
    emit progressFinished();
}

void GitWorker::cleanupState(Progress progress)
{
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
    emit progressFinished();
}
