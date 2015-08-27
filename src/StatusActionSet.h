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

class StatusActionSet : public bb::cascades::ActionSet
{
    Q_OBJECT
public:
    StatusActionSet(QObject *receiver, const char *translatorChangedSignal,
            const char *diffAction, const char *addAction,
            const char *resetAction, const char *selectAllAction);
    Q_SLOT void onTranslatorChanged();
private:
    bb::cascades::ActionItem *_diff, *_reset, *_add;
    bb::cascades::ActionItem *_selectAll;
};

#endif /* STATUSACTIONSET_H_ */
