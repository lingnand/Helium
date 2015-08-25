/*
 * StatusActionSet.h
 *
 *  Created on: Aug 12, 2015
 *      Author: lingnan
 */

#ifndef STATUSACTIONSET_H_
#define STATUSACTIONSET_H_

#include <bb/cascades/ActionSet>

namespace bb {
    namespace cascades {
        class ActionItem;
    }
}

class GitRepoPage;

enum StatusDiffType { HeadToIndex, IndexToWorkdir };

class StatusActionSet : public bb::cascades::ActionSet
{
    Q_OBJECT
public:
    StatusActionSet(GitRepoPage *, StatusDiffType);
    Q_SLOT void onTranslatorChanged();
private:
    GitRepoPage *_repoPage;
    bb::cascades::ActionItem *_diff, *_reset, *_add;
    bb::cascades::ActionItem *_selectAll;
    Q_SLOT void onSelectAllOnIndexTriggered();
    Q_SLOT void onSelectAllOnWorkdirTriggered();
};

#endif /* STATUSACTIONSET_H_ */
