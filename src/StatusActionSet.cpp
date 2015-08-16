/*
 * StatusActionSet.cpp
 *
 *  Created on: Aug 12, 2015
 *      Author: lingnan
 */

#include <QTimer>
#include <bb/cascades/ActionItem>
#include <bb/cascades/MultiSelectActionItem>
#include <bb/cascades/ListView>
#include <StatusActionSet.h>
#include <GitRepoPage.h>
#include <Utility.h>

using namespace bb::cascades;

StatusActionSet::StatusActionSet(GitRepoPage *page, const StatusDiffDelta &sdelta):
    _repoPage(page),
    _diff(ActionItem::create()
        .onTriggered(page, SLOT(showDiffSelection()))),
    _reset(NULL), _add(NULL),
    _selectAll(NULL)
{
    add(_diff);
    const char *method = NULL;
    switch (sdelta.type) {
        case HeadToIndex:
            add(_reset = ActionItem::create()
                .onTriggered(page, SLOT(resetSelections())));
            method = SLOT(onSelectAllOnIndexTriggered());
            break;
        case IndexToWorkdir:
            add(_add = ActionItem::create()
                .onTriggered(page, SLOT(addSelections())));
            method = SLOT(onSelectAllOnWorkdirTriggered());
            break;
    }
    if (method != NULL) {
        add(_selectAll = ActionItem::create()
            .imageSource(QUrl("asset:///images/ic_select_all.png"))
            .onTriggered(this, method));
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
    _selectAll->setTitle(tr("Select All"));
}

void StatusActionSet::onSelectAllOnIndexTriggered()
{
    _repoPage->statusListView()->multiSelectHandler()->setActive(true);
    QTimer::singleShot(0, _repoPage, SLOT(selectAllOnIndex()));
}

void StatusActionSet::onSelectAllOnWorkdirTriggered()
{
    _repoPage->statusListView()->multiSelectHandler()->setActive(true);
    QTimer::singleShot(0, _repoPage, SLOT(selectAllOnWorkdir()));
}
