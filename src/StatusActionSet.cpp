/*
 * StatusActionSet.cpp
 *
 *  Created on: Aug 12, 2015
 *      Author: lingnan
 */

#include <bb/cascades/ActionItem>
#include <StatusActionSet.h>
#include <Utility.h>

using namespace bb::cascades;

StatusActionSet::StatusActionSet(QObject *receiver, const char *translatorChangedSignal,
            const char *diffAction, const char *addAction,
            const char *resetAction, const char *selectAllAction):
    _diff(NULL), _add(NULL),
    _reset(NULL),_selectAll(NULL)
{
    if (diffAction)
        add(_diff = ActionItem::create()
            .onTriggered(receiver, diffAction));
    if (addAction)
        add(_add = ActionItem::create()
            .onTriggered(receiver, addAction));
    if (resetAction)
        add(_reset = ActionItem::create()
            .onTriggered(receiver, resetAction));
    if (selectAllAction)
        add(_selectAll = ActionItem::create()
            .imageSource(QUrl("asset:///images/ic_select_all.png"))
            .onTriggered(receiver, selectAllAction));
    onTranslatorChanged();
    conn(receiver, translatorChangedSignal,
            this, SLOT(onTranslatorChanged()));
}

void StatusActionSet::onTranslatorChanged()
{
    if (_diff)
        _diff->setTitle(tr("View Diff"));
    if (_add)
        _add->setTitle(tr("Add"));
    if (_reset)
        _reset->setTitle(tr("Reset"));
    if (_selectAll)
        _selectAll->setTitle(tr("Select All"));
}
