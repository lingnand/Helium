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

#ifndef MULTIVIEWPANE_H_
#define MULTIVIEWPANE_H_

#include <QList>
#include <bb/cascades/TabbedPane>
#include <Buffer.h>
#include <ShortcutHelp.h>

namespace bb {
    namespace cascades {
        class NavigationPane;
        class Shortcut;
    }
}

class Buffer;
class View;

class MultiViewPane : public bb::cascades::TabbedPane
{
    Q_OBJECT
public:
    MultiViewPane(QObject *parent=NULL);
    virtual ~MultiViewPane() {}
    View *activeView() const;
    // lastActiveView() is available even when views are hidden
    View *lastActiveView() const { return _lastActive; }
    // the active pane should always be a navigation pane
    bb::cascades::NavigationPane *activePane() const;
    Q_SLOT void setActiveTab(int, bool toast=false);
    Q_SLOT void setActiveTab(bb::cascades::Tab *, bool toast=false);
    Q_SLOT void setPrevTabActive();
    Q_SLOT void setNextTabActive();
    bb::cascades::Tab *at(int i) const; // the view at a given index
    int activeIndex(int offset=0) const; // the view with the given offset from the active one
    int indexOf(bb::cascades::Tab *) const; // give the index of the given view
    int count() const; // number of views
    void cloneActive(bool toast=true); // clone the current
    void removeView(View *, bool toast=true);
    Q_SLOT void addNewView(bool toast=true);
    void insertView(int index, View *view);
    Buffer *newBuffer();
    Buffer *bufferForFilepath(const QString &filepath);
    void removeBuffer(Buffer *buffer);
    Q_SLOT void onActiveTabChanged(bb::cascades::Tab *);
    Q_SLOT void onTranslatorChanged();
    void hideViews();
    void restoreViews();
    Q_SLOT void disableAllShortcuts();
    Q_SLOT void enableAllShortcuts();
    QList<ShortcutHelp> shortcutHelps();
Q_SIGNALS:
    void translatorChanged();
    void activeViewChanged(View *);
private:
    // used for determining the correct list of tabs to cycle
    int _base;
    bb::cascades::Tab *_newViewControl;
    bb::cascades::Shortcut *_newViewShortcut;

    QList<bb::cascades::Tab *> _save;
    View *_lastActive;
    // the list of buffers driving the views
    QList<Buffer *> _buffers;

    // whether the user can goToPrev/NextTab
    bool canTraverse();
    Q_SLOT void displayShortcuts();
};

#endif /* MULTIVIEWPANE_H_ */
