/*
 * DiffActionSet.cpp
 *
 *  Created on: Aug 24, 2015
 *      Author: lingnan
 */

#include <bb/cascades/ActionItem>
#include <DiffActionSet.h>
#include <Utility.h>

using namespace bb::cascades;

DiffActionSet::DiffActionSet(QObject *receiver, const char *translatorChangedSignal,
            const char *diffAction):
    _diff(NULL)
{
    if (diffAction)
        add(_diff = ActionItem::create()
            .onTriggered(receiver, diffAction));
    onTranslatorChanged();
    conn(receiver, translatorChangedSignal,
        this, SLOT(onTranslatorChanged()));
}

void DiffActionSet::onTranslatorChanged()
{
    if (_diff)
        _diff->setTitle(tr("View Diff"));
}

