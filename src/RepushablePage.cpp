/*
 * RepushablePage.cpp
 *
 *  Created on: May 26, 2015
 *      Author: lingnan
 */

#include <RepushablePage.h>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/NavigationPaneProperties>
#include <bb/cascades/ActionItem>
#include <Utility.h>

using namespace bb::cascades;

RepushablePage::RepushablePage(QObject *parent):
    Page(parent), _parent(parent)
{
    setPaneProperties(NavigationPaneProperties::create()
        .backButton(ActionItem::create()
            .onTriggered(this, SLOT(pop()))));
}

void RepushablePage::pop()
{
    setParent(_parent);
    emit toPop();
    emit exited();
}
