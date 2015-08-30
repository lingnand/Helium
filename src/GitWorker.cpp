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

GitWorker::GitWorker(LibQGit2::Repository *repo):
    _repo(repo)
{
}

void GitWorker::fetchStatusList(Progress progress)
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
        emit progressChanged(progress.current,
                bb::cascades::ProgressIndicatorState::Error, tr("Error fetching git status"));
    }
}

void GitWorker::addPathsAndFetchNewStatusList(const QList<QString> &paths, Progress progress)
{
    float initInc = (progress.cap-progress.current)/8;
    emit progressChanged(progress.current+=initInc);
    try {
        _repo->index().addAll(paths);
        emit progressChanged(progress.current+=initInc*2);
        fetchStatusList(progress);
    } catch (const LibQGit2::Exception &e) {
        qDebug() << "::::LIBQGIT2 ERROR when fetching status::::" << e.what();
        emit progressChanged(progress.current,
                bb::cascades::ProgressIndicatorState::Error, tr("Error fetching git status"));
    }
}

void GitWorker::resetPathsAndFetchNewStatusList(const QList<QString> &paths, Progress progress)
{
    float initInc = (progress.cap-progress.current)/8;
    emit progressChanged(initInc);
    try {
        _repo->index().removeAll(paths);
        emit progressChanged(progress.current+=initInc*2);
        fetchStatusList(progress);
    } catch (const LibQGit2::Exception &e) {
        qDebug() << "::::LIBQGIT2 ERROR when fetching status::::" << e.what();
        emit progressChanged(progress.current,
                bb::cascades::ProgressIndicatorState::Error, tr("Error fetching git status"));
    }
}
