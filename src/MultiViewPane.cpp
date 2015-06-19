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
#include <bb/cascades/NavigationPane>
#include <MultiViewPane.h>
#include <GeneralSettings.h>
#include <View.h>
#include <Buffer.h>
#include <Helium.h>
#include <Utility.h>

using namespace bb::cascades;

MultiViewPane::MultiViewPane(QObject *parent):
    TabbedPane(parent), _lastActive(NULL), _base(1),
    _newViewShortcut(Shortcut::create().key("c")
        .onTriggered(this, SLOT(addNewView()))),
    _newViewControl(Tab::create()
        .imageSource(QUrl("asset:///images/ic_add.png"))
        .onTriggered(this, SLOT(addNewView())))
{
    setShowTabsOnActionBar(false);
    Shortcut *prev = Shortcut::create().key("q")
        .onTriggered(this, SLOT(setPrevTabActive()));
    Shortcut *next = Shortcut::create().key("w")
        .onTriggered(this, SLOT(setNextTabActive()));
    Shortcut *help = Shortcut::create().key("Backspace")
        .onTriggered(this, SLOT(displayShortcuts()));
    addShortcut(prev);
    addShortcut(next);
    addShortcut(help);
    _newViewControl->addShortcut(_newViewShortcut);

    _newViewShortcut->setProperty("help", tr("New"));
    prev->setProperty("help", tr("Previous Tab Option"));
    next->setProperty("help", tr("Next Tab Option"));
    help->setProperty("help", tr("Display Shortcuts"));

    add(_newViewControl);

    conn(this, SIGNAL(activeTabChanged(bb::cascades::Tab*)),
            this, SLOT(onActiveTabChanged(bb::cascades::Tab*)));

    // load text
    onTranslatorChanged();
}

void MultiViewPane::disableAllShortcuts()
{
    TabbedPane::disableAllShortcuts();
    _newViewShortcut->setEnabled(false);
}

void MultiViewPane::enableAllShortcuts()
{
    TabbedPane::enableAllShortcuts();
    _newViewShortcut->setEnabled(true);
}

View *MultiViewPane::activeView() const
{
    return dynamic_cast<View *>(activeTab());
}

NavigationPane *MultiViewPane::activePane() const
{
    return (NavigationPane *) TabbedPane::activePane();
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
            Utility::toast(QString("%1/%2. %3").arg(index+1).arg(count()).arg(tab->title()));
    }
}

Tab *MultiViewPane::at(int i) const
{
    return TabbedPane::at(i+_base);
}

void MultiViewPane::hideViews()
{
    blockSignals(true);
    _newViewShortcut->setEnabled(false);
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
    _newViewShortcut->setEnabled(true);
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

bool MultiViewPane::canTraverse()
{
    if (count() == 1) {
        Utility::toast(tr("Only one tab"));
        return false;
    }
    return true;
}

void MultiViewPane::setPrevTabActive()
{
    int i = activeIndex(-1);
    if (i >= 0 && canTraverse())
        setActiveTab(i, true);
}

void MultiViewPane::setNextTabActive()
{
    int i = activeIndex(1);
    if (i >= 0 && canTraverse())
        setActiveTab(i, true);
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
    if (View *v = dynamic_cast<View *>(tab)) {
        if (v != _lastActive) {
            if (_lastActive) {
                _lastActive->onOutOfView();
            }
            View *old = _lastActive;
            _lastActive = v;
        }
    }
}

void MultiViewPane::onTranslatorChanged()
{
    _newViewControl->setTitle(tr("New"));
    emit translatorChanged();
}

QList<ShortcutHelp> MultiViewPane::shortcutHelps()
{
    QList<ShortcutHelp> helps;
    helps.append(ShortcutHelp::fromShortcut(_newViewShortcut));
    for (int i = 0; i < shortcutCount(); i++) {
        helps.append(ShortcutHelp::fromShortcut(shortcutAt(i)));
    }
    return helps;
}

void MultiViewPane::displayShortcuts()
{
    QList<ShortcutHelp> helps;
    Page *page = activePane()->top();
    for (int i = 0; i < page->shortcutCount(); i++)
        helps.append(ShortcutHelp::fromShortcut(page->shortcutAt(i)));
    // ActionItems
    for (int i = 0; i < page->actionCount(); i++)
        helps.append(ShortcutHelp::fromActionItem(page->actionAt(i)));
    // KeyListener
    for (int i = 0; i < page->keyListenerCount(); i++)
        helps.append(ShortcutHelp::fromKeyListener(page->keyListenerAt(i)));
    // pane properties
    helps.append(ShortcutHelp::fromPaneProperties(page->paneProperties()));
    // from multiViewPane itself
    helps.append(shortcutHelps());
    Utility::bigToast(ShortcutHelp::showAll(helps));
}
