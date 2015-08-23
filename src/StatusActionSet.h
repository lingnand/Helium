/*
 * StatusActionSet.h
 *
 *  Created on: Aug 12, 2015
 *      Author: lingnan
 */

#ifndef STATUSACTIONSET_H_
#define STATUSACTIONSET_H_

#include <bb/cascades/ActionSet>
#include <libqgit2/qgitdiffdelta.h>

namespace bb {
    namespace cascades {
        class ActionItem;
    }
}

class GitRepoPage;

enum StatusDiffType { HeadToIndex, IndexToWorkdir };

struct StatusDiffDelta {
    StatusDiffType type;
    LibQGit2::DiffDelta delta;
    StatusDiffDelta(StatusDiffType t=HeadToIndex, const LibQGit2::DiffDelta &d=LibQGit2::DiffDelta()):
        type(t), delta(d) {}
};

Q_DECLARE_METATYPE(StatusDiffDelta)

class StatusActionSet : public bb::cascades::ActionSet
{
    Q_OBJECT
public:
    StatusActionSet(GitRepoPage *, const StatusDiffDelta &);
    Q_SLOT void onTranslatorChanged();
private:
    GitRepoPage *_repoPage;
    bb::cascades::ActionItem *_diff, *_reset, *_add;
    bb::cascades::ActionItem *_selectAll;
    Q_SLOT void onSelectAllOnIndexTriggered();
    Q_SLOT void onSelectAllOnWorkdirTriggered();
};

#endif /* STATUSACTIONSET_H_ */
