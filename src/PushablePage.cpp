/*
 * PushablePage.cpp
 *
 *  Created on: May 26, 2015
 *      Author: lingnan
 */

#include <PushablePage.h>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/NavigationPaneProperties>
#include <bb/cascades/ActionItem>
#include <bb/cascades/Shortcut>
#include <Utility.h>

using namespace bb::cascades;

PushablePage::PushablePage()
{
    setPaneProperties(NavigationPaneProperties::create()
        .backButton(ActionItem::create()
            .addShortcut(Shortcut::create().key("x"))
            .onTriggered(this, SLOT(pop()))));
}

NavigationPane *PushablePage::parent() const {
    return (NavigationPane *) Page::parent();
}

void PushablePage::pop() {
    parent()->pop();
}
