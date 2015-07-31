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
#include <bb/system/SystemUiResult>
#include <Buffer.h>
#include <ShortcutHelp.h>
#include <Zipper.h>

namespace bb {
    namespace cascades {
        class NavigationPane;
        class Shortcut;
        class KeyEvent;
        namespace pickers {
            class FilePicker;
        }
    }
}

class Buffer;
class View;
class Project;

class MultiViewPane : public bb::cascades::TabbedPane
{
    Q_OBJECT
public:
    MultiViewPane(Zipper<Project *> *projects, QObject *parent=NULL);
    virtual ~MultiViewPane() {}
    Project *activeProject() const { return _projects->active(); }
    bb::cascades::NavigationPane *activePane() const {
        return (bb::cascades::NavigationPane *) TabbedPane::activePane();
    }
    // focus to the current view and hide all other tabs
    void zoomIntoView();
    void zoomOutOfView();
    void setActiveTabWithToast(bb::cascades::Tab *, bool toast);
    Q_SLOT void setPrevTabActive();
    Q_SLOT void setNextTabActive();
    bb::cascades::Shortcut *newViewShortcut() const { return _newViewShortcut; }
    bb::cascades::Shortcut *prevTabShortcut() const { return _prevTabShortcut; }
    bb::cascades::Shortcut *nextTabShortcut() const { return _nextTabShortcut; }
    Q_SLOT void onTranslatorChanged();
    Q_SLOT void removeActiveProject();
    // relay to current project
    Q_SLOT void createEmptyView();
    bool enterKeyPressedOnTopScope() const { return _enterKeyPressedOnTopScope; }
Q_SIGNALS:
    void translatorChanged();
private:
    Zipper<Project *> *_projects;
    bb::cascades::Shortcut *_newProjectShortcut;
    bb::cascades::Tab *_newProjectControl;
    bb::cascades::Shortcut *_newViewShortcut;
    bb::cascades::Tab *_newViewControl;
    bb::cascades::Shortcut *_prevTabShortcut;
    bb::cascades::Shortcut *_nextTabShortcut;
    bb::cascades::Shortcut *_nextProjectShortcut;
    bb::cascades::Shortcut *_changeProjectPathShortcut;
    bb::cascades::pickers::FilePicker *_fpicker;
    bool _zoomed;
    bool _reopenSidebar;
    bool _enterKeyPressedOnTopScope;

    bb::cascades::pickers::FilePicker *filePicker(const QString &directory,
            QObject *target,
            const char *onFileSelected, const char *onCancelled=NULL);
    Q_SLOT void setNextProjectActive();
    Q_SLOT void createProject();
    // monitoring for the current project
    Q_SLOT void onProjectViewInserted(int, View *);
    Q_SLOT void onProjectViewRemoved(View *);
    Q_SLOT void onProjectActiveViewChanged(int index, View *);
    Q_SLOT void onNewProjectPathSelected(const QStringList &);
    Q_SLOT void onProjectTriggered();
    Q_SLOT void onProjectPathSelected(const QStringList &);
    // monitoring zipper
    Q_SLOT void onProjectInserted(int, Project *);
    Q_SLOT void onProjectRemoved(int, Project *);
    Q_SLOT void onActiveProjectChanged(Project *change, Project *old=NULL);

    Q_SLOT void changeProjectPath();
    bool sidebarHidden() const {
        return sidebarVisualState() == bb::cascades::SidebarVisualState::Hidden;
    }
    Q_SLOT void resetProjectActiveView(bool toast=false);
    Q_SLOT void setProjectActiveView(Project *, int viewIndex, View *, bool toast);

    void setActiveTabIndex(int, bool toast);
    void setActiveTabWithOffset(int, bool toast);

    Q_SLOT void onSidebarVisualStateChanged(bb::cascades::SidebarVisualState::Type);
    Q_SLOT void onUnsavedChangeDialogFinishedWhenClosingProject(bb::system::SystemUiResult::Type);
    Q_SLOT void displayShortcuts();

    Q_SLOT void onModifiedKey(bb::cascades::KeyEvent *);
    Q_SLOT void onModKey(bb::cascades::KeyEvent *);
    Q_SLOT void flagEnterKeyOnTopScope();
};

#endif /* MULTIVIEWPANE_H_ */
