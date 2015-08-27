/*
 * CommitActionSet.h
 *
 *  Created on: Aug 25, 2015
 *      Author: lingnan
 */

#ifndef COMMITACTIONSET_H_
#define COMMITACTIONSET_H_

#include <bb/cascades/ActionSet>

namespace bb {
    namespace cascades {
        class ActionItem;
    }
}

class CommitActionSet : public bb::cascades::ActionSet
{
    Q_OBJECT
public:
    CommitActionSet(QObject *receiver, const char *translatorChangedSignal,
            const char *infoAction, const char *checkoutAction);
    Q_SLOT void onTranslatorChanged();
private:
    bb::cascades::ActionItem *_info, *_checkout;
};

#endif /* COMMITACTIONSET_H_ */
