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

#include <bb/cascades/Application>
#include <bb/cascades/TabbedPane>
#include <bb/cascades/Tab>
#include <bb/cascades/Shortcut>
#include <bb/cascades/Page>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/pickers/FilePicker>
#include <MultiViewPane.h>
#include <GeneralSettings.h>
#include <View.h>
#include <Buffer.h>
#include <Project.h>
#include <Helium.h>
#include <Utility.h>

using namespace bb::cascades;

MultiViewPane::MultiViewPane(QObject *parent):
    TabbedPane(parent),
    _newProjectShortcut(Shortcut::create().key("i")
        .onTriggered(this, SLOT(addNewProjectAndSetActive()))),
    _newProjectControl(Tab::create()
        .imageSource(QUrl("asset:///images/ic_add_folder.png"))
        .onTriggered(this, SLOT(addNewProjectAndSetActive()))),
    _newViewShortcut(Shortcut::create().key("c")
        .onTriggered(this, SLOT(addNewView()))),
    _newViewControl(Tab::create()
        .imageSource(QUrl("asset:///images/ic_add.png"))
        .onTriggered(this, SLOT(addNewView()))),
    _prevTabShortcut(Shortcut::create().key("q")
        .onTriggered(this, SLOT(setPrevTabActive()))),
    _nextTabShortcut(Shortcut::create().key("w")
        .onTriggered(this, SLOT(setNextTabActive()))),
    _nextProjectShortcut(Shortcut::create().key("n")
        .onTriggered(this, SLOT(setNextProjectActive()))),
    _changeProjectPathShortcut(Shortcut::create().key("o")
        .onTriggered(this, SLOT(changeProjectPath()))),
    _helpShortcut(Shortcut::create().key("Backspace")
        .onTriggered(this, SLOT(displayShortcuts()))),
    _activeProject(NULL),
    _fpicker(NULL), _zoomed(false), _reopenSidebar(false)
{
    setShowTabsOnActionBar(false);
    addShortcut(_prevTabShortcut);
    addShortcut(_nextTabShortcut);
    // project management
    addShortcut(_nextProjectShortcut);
    addShortcut(_changeProjectPathShortcut);
    addShortcut(_helpShortcut);
    _newProjectControl->addShortcut(_newProjectShortcut);
    _newViewControl->addShortcut(_newViewShortcut);

    // set up the controls
    add(_newProjectControl);
    add(_newViewControl);

    Project *p = new Project("/accounts/1000/removable/sdcard");
    insertProject(0, p);
    setActiveProject(p, false);

    conn(this, SIGNAL(sidebarVisualStateChanged(bb::cascades::SidebarVisualState::Type)),
        this, SLOT(onSidebarVisualStateChanged(bb::cascades::SidebarVisualState::Type)));

    // load text
    onTranslatorChanged();
}

void MultiViewPane::onProjectTriggered()
{
    Project *src = (Project *) sender();
    if (_activeProject == src)
        changeProjectPath();
    else {
        setActiveProject(src, false);
        _reopenSidebar = true;
    }
}

void MultiViewPane::changeProjectPath()
{
    filePicker(_activeProject->path(), this,
            SLOT(onProjectPathSelected(const QStringList&)),
            SLOT(resetProjectActiveView()))->open();
}

void MultiViewPane::onProjectPathSelected(const QStringList &list)
{
    _activeProject->setPath(list[0]);
    resetProjectActiveView(true);
}

void MultiViewPane::resetProjectActiveView(bool toast)
{
    setActiveTab(_activeProject->activeView());
    if (toast)
        Utility::toast(QString("[%1/%2. %3]\n%4/%5. %6")
                .arg(_projects.indexOf(_activeProject)+1)
                .arg(_projects.size())
                .arg(_activeProject->title())
                .arg(_activeProject->activeViewIndex()+1)
                .arg(_activeProject->size())
                .arg(_activeProject->activeView()->title()));
}

void MultiViewPane::disableAllShortcuts()
{
    TabbedPane::disableAllShortcuts();
    _newProjectShortcut->setEnabled(false);
    _newViewShortcut->setEnabled(false);
}

void MultiViewPane::enableAllShortcuts()
{
    TabbedPane::enableAllShortcuts();
    _newProjectShortcut->setEnabled(false);
    _newViewShortcut->setEnabled(true);
}

void MultiViewPane::setActiveProject(Project *p, bool toast)
{
    if (p != _activeProject) {
        if (_activeProject) {
            _activeProject->unselect();
            disconn(_activeProject, SIGNAL(viewInserted(int, View*)),
                this, SLOT(onProjectViewInserted(int, View*)));
            disconn(_activeProject, SIGNAL(viewRemoved(View*)),
                this, SLOT(onProjectViewRemoved(View*)));
            disconn(_activeProject, SIGNAL(activeViewChanged(View*, bool)),
                this, SLOT(onProjectActiveViewChanged(View*, bool)));
            for (int i = 0; i < _activeProject->size(); i++)
                remove(_activeProject->at(i));
        }
        _activeProject = p;
        _activeProject->select();
        for (int i = 0; i < _activeProject->size(); i++)
            add(_activeProject->at(i));
        resetProjectActiveView(toast);
        conn(_activeProject, SIGNAL(viewInserted(int, View*)),
            this, SLOT(onProjectViewInserted(int, View*)));
        conn(_activeProject, SIGNAL(viewRemoved(View*)),
            this, SLOT(onProjectViewRemoved(View*)));
        conn(_activeProject, SIGNAL(activeViewChanged(View*, bool)),
            this, SLOT(onProjectActiveViewChanged(View*, bool)));
    }
}

void MultiViewPane::setNextProjectActive()
{
    if (_projects.size() < 2)
        resetProjectActiveView(true);
    else
        setActiveProject(
                _projects[PMOD(_projects.indexOf(_activeProject)+1, _projects.size())],
                true);
}

void MultiViewPane::insertProject(int index, Project *project)
{
    _projects.insert(index, project);
    insert(1+index, project);
    conn(this, SIGNAL(translatorChanged()),
        project, SIGNAL(translatorChanged()));
    conn(project, SIGNAL(triggered()),
        this, SLOT(onProjectTriggered()));
}

void MultiViewPane::removeActiveProject()
{
    QStringList files;
    for (int i = 0; i < _activeProject->size(); i++) {
        if (_activeProject->at(i)->unsafeToRemove())
            files.append(QString("\t%1").arg(_activeProject->at(i)->title()));
    }
    if (!files.empty()) {
        Utility::dialog(tr("Yes"), tr("No"), tr("Unsaved change detected"),
                tr("These files have unsaved changes:\n"
                "\n%1\n\n"
                "Do you want to continue?").arg(files.join("\n")),
                this, SLOT(onUnsavedChangeDialogFinishedWhenClosingProject(bb::system::SystemUiResult::Type)));
    } else {
        removeAt(_projects.indexOf(_activeProject));
    }
}

void MultiViewPane::onUnsavedChangeDialogFinishedWhenClosingProject(bb::system::SystemUiResult::Type type)
{
    if (type == bb::system::SystemUiResult::ConfirmButtonSelection) {
        removeAt(_projects.indexOf(_activeProject));
    }
}

void MultiViewPane::removeAt(int index)
{
    Project *project = _projects[index];
    _projects.removeAt(index);
    remove(project);
    project->deleteLater();
    if (project == _activeProject) {
        if (_projects.isEmpty()) {
            insertProject(0, new Project(project->path()));
        }
        setActiveProject(_projects[qMin(index, _projects.size()-1)], true);
    }
}

pickers::FilePicker *MultiViewPane::filePicker(const QString &directory,
            QObject *target,
            const char *onFileSelected, const char *onCancelled)
{
    if (!_fpicker) {
        _fpicker = new pickers::FilePicker(this);
        _fpicker->setMode(pickers::FilePickerMode::SaverMultiple);
    }
    _fpicker->setDirectories(QStringList(directory));
    _fpicker->disconnect();
    conn(_fpicker, SIGNAL(fileSelected(const QStringList&)), target, onFileSelected);
    if (onCancelled) {
        conn(_fpicker, SIGNAL(canceled()), target, onCancelled);
    }
    return _fpicker;
}

void MultiViewPane::addNewProjectAndSetActive()
{
    filePicker(_activeProject->path(), this,
            SLOT(onNewProjectPathSelected(const QStringList&)),
            SLOT(resetProjectActiveView()))->open();
}

void MultiViewPane::onNewProjectPathSelected(const QStringList &list)
{
    int i = _projects.size();
    Project *p = new Project(list[0]);
    insertProject(i, p);
    setActiveProject(p, true);
}

void MultiViewPane::onProjectViewInserted(int index, View *view)
{
    insert(2+_projects.size()+index, view);
}

void MultiViewPane::onProjectViewRemoved(View *view)
{
    remove(view);
}

void MultiViewPane::onProjectActiveViewChanged(View *, bool triggeredFromSidebar)
{
    resetProjectActiveView(!triggeredFromSidebar);
}

void MultiViewPane::zoomIntoView()
{
    if (!_zoomed) {
        _zoomed = true;
        while (count() > 0)
            remove(at(0));
        // set the active pane to use the current one
        setActivePane(_activeProject->activeView()->detachContent());
    }
}

void MultiViewPane::zoomOutOfView()
{
    if (_zoomed) {
        _zoomed = false;
        while (count() > 0)
            remove(at(0));
        // add the controls
        setActivePane(NULL);
        _activeProject->activeView()->reattachContent();
        add(_newProjectControl);
        for (int i = 0; i < _projects.size(); i++)
            add(_projects[i]);
        // add views
        add(_newViewControl);
        // add the views in the projects back
        for (int i = 0; i < _activeProject->size(); i++)
            add(_activeProject->at(i));
        resetProjectActiveView();
    }
}

void MultiViewPane::addNewView()
{
    _activeProject->addNewViewAndSetActive();
}

void MultiViewPane::setActiveTabWithToast(Tab *tab, bool toast)
{
    setActiveTabIndex(indexOf(tab), toast);
}

void MultiViewPane::setActiveTabIndex(int index, bool toast)
{
    Tab *tab = at(index);
    setActiveTab(tab);
    if (toast)
        Utility::toast(QString("%1/%2. %3")
                .arg(index+1).arg(count()).arg(tab->title()));
}

void MultiViewPane::setActiveTabWithOffset(int offset, bool toast)
{
    if (_zoomed)
        setActiveTabIndex(PMOD(indexOf(activeTab())+offset, count()), toast);
    else if (_activeProject->size() < 2) // refresh display
        resetProjectActiveView(true);
    else
        _activeProject->setActiveViewIndex(
                PMOD(_activeProject->activeViewIndex()+offset,
                        _activeProject->size()));
}

void MultiViewPane::setPrevTabActive()
{
    setActiveTabWithOffset(-1, true);
}

void MultiViewPane::setNextTabActive()
{
    setActiveTabWithOffset(1, true);
}

void MultiViewPane::onSidebarVisualStateChanged(SidebarVisualState::Type type)
{
    if (type == SidebarVisualState::Hidden && _reopenSidebar) {
        _reopenSidebar = false;
        setSidebarState(SidebarState::VisibleFull);
    }
}

void MultiViewPane::onTranslatorChanged()
{
    _newProjectControl->setTitle(tr("New Project"));
    _newViewControl->setTitle(tr("New"));
    _newProjectShortcut->setProperty("help", tr("New Project"));
    _newViewShortcut->setProperty("help", tr("New"));
    _prevTabShortcut->setProperty("help", tr("Previous Tab/Option"));
    _nextTabShortcut->setProperty("help", tr("Next Tab/Option"));
    _nextProjectShortcut->setProperty("help", tr("Next Project"));
    _changeProjectPathShortcut->setProperty("help", tr("Change Project Path"));
    _helpShortcut->setProperty("help", tr("Display Shortcuts"));
    emit translatorChanged();
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
    helps.append(ShortcutHelp::fromShortcut(_newProjectShortcut));
    helps.append(ShortcutHelp::fromShortcut(_newViewShortcut));
    for (int i = 0; i < shortcutCount(); i++)
        helps.append(ShortcutHelp::fromShortcut(shortcutAt(i)));
    Utility::dialog(tr("Dismiss"), tr("Shortcuts"), ShortcutHelp::showAll(helps));
}
