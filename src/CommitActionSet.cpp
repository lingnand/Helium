/*
 * CommitActionSet.cpp
 *
 *  Created on: Aug 25, 2015
 *      Author: lingnan
 */

#include <bb/cascades/ActionItem>
#include <CommitActionSet.h>
#include <Utility.h>

using namespace bb::cascades;

CommitActionSet::CommitActionSet(QObject *receiver, const char *translatorChangedSignal,
            const char *infoAction, const char *checkoutAction):
    _info(NULL),
    _checkout(NULL)
{
    if (infoAction)
        add(_info = ActionItem::create()
            .onTriggered(receiver, infoAction));
    if (checkoutAction)
        add(_checkout = ActionItem::create()
            .onTriggered(receiver, checkoutAction));
    onTranslatorChanged();
    conn(receiver, translatorChangedSignal,
            this, SLOT(onTranslatorChanged()));
}

void CommitActionSet::onTranslatorChanged()
{
    if (_info)
        _info->setTitle(tr("View Info"));
    if (_checkout)
        _checkout->setTitle(tr("Checkout"));
}

