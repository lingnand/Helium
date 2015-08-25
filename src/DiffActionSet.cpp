/*
 * DiffActionSet.cpp
 *
 *  Created on: Aug 24, 2015
 *      Author: lingnan
 */

#include <bb/cascades/ActionItem>
#include <DiffActionSet.h>
#include <GitCommitInfoPage.h>
#include <Utility.h>

using namespace bb::cascades;

DiffActionSet::DiffActionSet(GitCommitInfoPage *page):
    _diff(ActionItem::create()
        .onTriggered(page, SLOT(showDiffSelection())))
{
    add(_diff);
    onTranslatorChanged();
    conn(page, SIGNAL(translatorChanged()),
        this, SLOT(onTranslatorChanged()));
}

void DiffActionSet::onTranslatorChanged()
{
    _diff->setTitle(tr("View Diff"));
}

