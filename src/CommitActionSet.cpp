/*
 * CommitActionSet.cpp
 *
 *  Created on: Aug 25, 2015
 *      Author: lingnan
 */

#include <bb/cascades/ActionItem>
#include <CommitActionSet.h>
#include <GitLogPage.h>
#include <Utility.h>

using namespace bb::cascades;

CommitActionSet::CommitActionSet(GitLogPage *page):
    _info(ActionItem::create()
        .onTriggered(page, SLOT(showCommitInfoSelection()))),
    _checkout(ActionItem::create()
        .onTriggered(page, SLOT(checkoutSelection())))
{
    add(_info);
    add(_checkout);
    onTranslatorChanged();
    conn(page, SIGNAL(translatorChanged()),
            this, SLOT(onTranslatorChanged()));
}

void CommitActionSet::onTranslatorChanged()
{
    _info->setTitle(tr("View Info"));
    _checkout->setTitle(tr("Checkout"));
}

