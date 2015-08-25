/*
 * DiffActionSet.h
 *
 *  Created on: Aug 24, 2015
 *      Author: lingnan
 */

#ifndef DIFFACTIONSET_H_
#define DIFFACTIONSET_H_

#include <bb/cascades/ActionSet>

namespace bb {
    namespace cascades {
        class ActionItem;
    }
}

class GitCommitInfoPage;

class DiffActionSet : public bb::cascades::ActionSet
{
    Q_OBJECT
public:
    DiffActionSet(GitCommitInfoPage *);
    Q_SLOT void onTranslatorChanged();
private:
    bb::cascades::ActionItem *_diff;
};

#endif /* DIFFACTIONSET_H_ */
