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
        .imageSource(QUrl("asset:///images/ic_add.png"))
        .addShortcut(bb::cascades::Shortcut::create().key("c")
                .onTriggered(this, SLOT(addNewView())))
        .onTriggered(this, SLOT(addNewView())));
}

View *MultiViewPane::activeView() const
{
    bb::cascades::Tab *tab = activeTab();
    if (tab == newViewControl())
        return NULL;
    return (View *) activeTab();
}

void MultiViewPane::setActiveView(View *view, bool toast)
{
    View *active = activeView();
    if (active != view) {
        setActiveTab(view);
        if (active)
            active->onOutOfView();
        if (toast)
            Utility::toast(view->title());
    }
}

bb::cascades::Tab *MultiViewPane::newViewControl() const
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

void MultiViewPane::addNewView(bool toast)
{
    // defaults to open a new buffer
    add(new View(newBuffer()), toast);
}

void MultiViewPane::connectView(View *view)
{
    conn(this, SIGNAL(translatorChanged()), view, SLOT(onTranslatorChanged()));
}

void MultiViewPane::insert(int index, View *view, bool toast)
{
    connectView(view);
    bb::cascades::TabbedPane::insert(index+1, view);
    setActiveView(view, toast);
}

void MultiViewPane::add(View *view, bool toast)
{
    connectView(view);
    bb::cascades::TabbedPane::add(view);
    setActiveView(view, toast);
}

void MultiViewPane::cloneActive(bool toast)
{
    insert(indexOf(activeView())+1, new View(activeView()->buffer()), toast);
}

void MultiViewPane::remove(View *view)
{
    if (view == activeView()) {
        if (count() == 1) {
            // better to create a new view
            addNewView();
        } else {
            // activate the view before it
            setActiveView(atOffset(-1));
        }
    }
    view->setBuffer(NULL);
    bb::cascades::TabbedPane::remove(view);
    view->deleteLater();
}

void MultiViewPane::setPrevViewActive(bool toast)
{
    if (count() == 0)
        return;
    setActiveView(atOffset(-1), toast);
}

void MultiViewPane::setNextViewActive(bool toast)
{
    if (count() == 0)
        return;
    setActiveView(atOffset(1), toast);
}

View *MultiViewPane::atOffset(int offset) const
{
    return at(PMOD(indexOf(activeView()) + offset, count()));
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

void MultiViewPane::onTranslatorChanged()
{
    newViewControl()->setTitle(tr("New"));
    emit translatorChanged();
}
