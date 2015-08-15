/*
 * StatusActionSet.cpp
 *
 *  Created on: Aug 12, 2015
 *      Author: lingnan
 */

#include <bb/cascades/ActionItem>
#include <StatusActionSet.h>
#include <GitRepoPage.h>
#include <Utility.h>

using namespace bb::cascades;

StatusActionSet::StatusActionSet(GitRepoPage *page, const StatusDiffDelta &sdelta):
    _diff(ActionItem::create()
        .onTriggered(page, SLOT(diffSelection()))),
    _reset(NULL), _add(NULL)
{
    add(_diff);
    switch (sdelta.type) {
        case HeadToIndex:
            add(_reset = ActionItem::create()
                .onTriggered(page, SLOT(resetSelections())));
            break;
        case IndexToWorkdir:
            add(_add = ActionItem::create()
                .onTriggered(page, SLOT(addSelections())));
            break;
    }
    onTranslatorChanged();
    conn(page, SIGNAL(translatorChanged()),
            this, SLOT(onTranslatorChanged()));
}

void StatusActionSet::onTranslatorChanged()
{
    _diff->setTitle(tr("View Diff"));
    if (_add)
        _add->setTitle(tr("Add"));
    if (_reset)
        _reset->setTitle(tr("Reset"));
}

