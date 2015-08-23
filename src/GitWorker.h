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
    Q_SLOT void addPathsAndFetchNewStatusList(const QList<QString> &, Progress progress=Progress());
    Q_SLOT void resetPathsAndFetchNewStatusList(const QList<QString> &, Progress progress=Progress());
Q_SIGNALS:
    void progressChanged(float progress, bb::cascades::ProgressIndicatorState::Type state=bb::cascades::ProgressIndicatorState::Progress, const QString &msg=QString());
    void statusListFetched(const LibQGit2::StatusList &);
private:
    LibQGit2::Repository *_repo;
};

#endif /* GITWORKER_H_ */
