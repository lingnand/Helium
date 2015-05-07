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
#include <bb/cascades/Page>
#include <MultiViewPane.h>
#include <View.h>
#include <Buffer.h>
#include <Utility.h>

using namespace bb::cascades;

MultiViewPane::MultiViewPane(QObject *parent):
        TabbedPane(parent), _lastActive(NULL), _base(1)
{
    setShowTabsOnActionBar(false);
    addShortcut(Shortcut::create().key("q")
            .onTriggered(this, SLOT(setPrevTabActive())));
    addShortcut(Shortcut::create().key("w")
            .onTriggered(this, SLOT(setNextTabActive())));

    // TODO: handle auto focus properly: focus on the textarea when the tab is switched to
    // However, think about the case when you press enter+<switching between tabs> and you want to do it again on the next switched to tab
    add(Tab::create()
        .imageSource(QUrl("asset:///images/ic_add.png"))
        .addShortcut(Shortcut::create().key("c")
                .onTriggered(this, SLOT(addNewView())))
        .onTriggered(this, SLOT(addNewView())));

    conn(this, SIGNAL(activeTabChanged(bb::cascades::Tab*)),
            this, SLOT(onActiveTabChanged(bb::cascades::Tab*)));

    // load text
    onTranslatorChanged();
}

Tab *MultiViewPane::newViewControl() const
{
    return TabbedPane::at(0);
}

View *MultiViewPane::activeView() const
{
    Tab *tab = activeTab();
    return tab == newViewControl() ? NULL : (View *) tab;
}

void MultiViewPane::setActiveTab(Tab *tab, bool toast)
{
    setActiveTab(indexOf(tab), toast);
}

void MultiViewPane::setActiveTab(int index, bool toast)
{
    Tab *active = activeTab(), *tab = at(index);
    if (active != tab) {
        TabbedPane::setActiveTab(tab);
        if (toast)
            // TODO: maybe not include index when we are cycling between options
            Utility::toast(QString("%1. %2").arg(index).arg(tab->title()));
    }
}

Tab *MultiViewPane::at(int i) const
{
    return TabbedPane::at(i+_base);
}

void MultiViewPane::hideViews()
{
    blockSignals(true);
    _base = 0;
    while (count() > 0) {
        Tab *tab = at(0);
        _save.append(tab);
        remove(tab);
    }
}

void MultiViewPane::restoreViews()
{
    while (count() > 0) {
        remove(at(0));
    }
    for (int i = 0; i < _save.size(); i++) {
        add(_save[i]);
    }
    _save.clear();
    setActiveTab(_lastActive);
    _base = 1;
    blockSignals(false);
}

int MultiViewPane::indexOf(Tab *tab) const
{
    return TabbedPane::indexOf(tab) - _base;
}

int MultiViewPane::count() const
{
    return TabbedPane::count() - _base;
}

void MultiViewPane::addNewView(bool toast)
{
    int i = count();
    View *v = new View(newBuffer());
    insertView(i, v);
    setActiveTab(i, toast);
}

void MultiViewPane::insertView(int index, View *view)
{
    conn(this, SIGNAL(translatorChanged()), view, SLOT(onTranslatorChanged()));
    insert(index+_base, view);
}

void MultiViewPane::cloneActive(bool toast)
{
    int i = activeIndex()+1;
    insertView(i, new View(activeView()->buffer()));
    setActiveTab(i, toast);
}

void MultiViewPane::removeView(View *view, bool toast)
{
    Tab *activateLater = NULL;
    if (view == activeView()) {
        if (count() == 1) {
            // better to create a new view
            View *v = new View(newBuffer());
            insertView(1, v);
            activateLater = v;
        } else {
            // activate the view before it (or after it if it's the first one)
            int ai = activeIndex();
            if (ai == 0) {
                ai = 1;
            } else {
                ai--;
            }
            activateLater = at(ai);
        }
    }
    view->setBuffer(NULL);
    remove(view);
    view->deleteLater();
    if (activateLater)
        setActiveTab(activateLater, toast);
}

void MultiViewPane::setPrevTabActive(bool toast)
{
    int i = activeIndex(-1);
    if (i >= 0)
        setActiveTab(i, toast);
}

void MultiViewPane::setNextTabActive(bool toast)
{
    int i = activeIndex(1);
    if (i >= 0)
        setActiveTab(i, toast);
}

int MultiViewPane::activeIndex(int offset) const
{
    int i = indexOf(activeTab());
    if (count() == 0 || i < 0)
        return -1;
    return PMOD(i+offset, count());
}

Buffer *MultiViewPane::newBuffer()
{
    Buffer *b = new Buffer(100, this);
    _buffers.append(b);
    return b;
}

Buffer *MultiViewPane::bufferForFilepath(const QString &filepath)
{
    for (int i = 0; i < _buffers.size(); i++) {
        if (_buffers[i]->filepath() == filepath) {
            return _buffers[i];
        }
    }
    return NULL;
}

void MultiViewPane::removeBuffer(Buffer *buffer)
{
    if (_buffers.removeOne(buffer)) {
        buffer->deleteLater();
    }
}

void MultiViewPane::onActiveTabChanged(Tab *tab)
{
    if (tab != newViewControl() && tab != _lastActive) {
        if (_lastActive) {
            _lastActive->onOutOfView();
        }
        _lastActive = (View *) tab;
    }
}

void MultiViewPane::onTranslatorChanged()
{
    newViewControl()->setTitle(tr("New"));
    emit translatorChanged();
}
