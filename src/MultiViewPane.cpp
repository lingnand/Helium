/*
 * Copyright (c) 2011-2014 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <QSignalMapper>
#include <bb/cascades/Application>
#include <bb/cascades/TabbedPane>
#include <bb/cascades/Tab>
#include <bb/cascades/Shortcut>
#include <MultiViewPane.h>
#include <View.h>
#include <Buffer.h>
#include <Utility.h>

MultiViewPane::MultiViewPane()
{
    setShowTabsOnActionBar(false);
    addShortcut(bb::cascades::Shortcut::create().key("q")
            .onTriggered(this, SLOT(setPrevViewActive())));
    addShortcut(bb::cascades::Shortcut::create().key("w")
            .onTriggered(this, SLOT(setNextViewActive())));

    // TODO: handle auto focus properly: focus on the textarea when the tab is switched to
    // However, think about the case when you press enter+<switching between tabs> and you want to do it again on the next switched to tab
    bb::cascades::TabbedPane::add(bb::cascades::Tab::create()
        .imageSource(QUrl("asset:///images/ic_compose.png"))
        .addShortcut(bb::cascades::Shortcut::create().key("c")
                .onTriggered(this, SLOT(addNewView())))
        .onTriggered(this, SLOT(addNewView())));
}

View *MultiViewPane::activeView() const
{
    return (View *) activeTab();
}

void MultiViewPane::setActiveView(View *view)
{
    setActiveTab(view);
}

bb::cascades::Tab *MultiViewPane::newViewControl()
{
    return bb::cascades::TabbedPane::at(0);
}

View *MultiViewPane::at(int i) const
{
    return (View *) bb::cascades::TabbedPane::at(i+1);
}

int MultiViewPane::indexOf(View *view) const
{
    return bb::cascades::TabbedPane::indexOf(view) - 1;
}

int MultiViewPane::count() const
{
    return bb::cascades::TabbedPane::count() - 1;
}

void MultiViewPane::addNewView()
{
    add(new View(new Buffer(100)));
}

void MultiViewPane::add(View *view)
{
    View *active = NULL;
    if (count() > 0) {
        active = activeView();
    }
    conn(this, SIGNAL(translatorChanged()), view, SLOT(onTranslatorChanged()));
    bb::cascades::TabbedPane::add(view);
    setActiveView(view);
    if (active)
        active->onOutOfView();
}

void MultiViewPane::setPrevViewActive()
{
    if (count() == 0)
        return;
    setActiveView(atOffset(-1));
}

void MultiViewPane::setNextViewActive()
{
    if (count() == 0)
        return;
    setActiveView(atOffset(1));
}

View *MultiViewPane::atOffset(int offset) const
{
    return at(PMOD(indexOf(activeView()) + offset, count()));
}

void MultiViewPane::onTranslatorChanged()
{
    newViewControl()->setTitle(tr("New"));
    emit translatorChanged();
}
