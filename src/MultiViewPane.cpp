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
#include <bb/cascades/KeyEvent>
#include <bb/cascades/pickers/FilePicker>
#include <MultiViewPane.h>
#include <GeneralSettings.h>
#include <View.h>
#include <Buffer.h>
#include <Project.h>
#include <Helium.h>
#include <AppearanceSettings.h>
#include <Utility.h>
#include <ModKeyListener.h>

using namespace bb::cascades;

MultiViewPane::MultiViewPane(Zipper<Project *> *projects, QObject *parent):
    TabbedPane(parent),
    _projects(projects),
    _newProjectShortcut(Shortcut::create().key("m")
        .onTriggered(this, SLOT(createProject()))),
    _newProjectControl(Tab::create()
        .imageSource(QUrl("asset:///images/ic_add_folder.png"))
        .onTriggered(this, SLOT(createProject()))),
    _newViewShortcut(Shortcut::create().key("c")
        .onTriggered(this, SLOT(createEmptyView()))),
    _newViewControl(Tab::create()
        .imageSource(QUrl("asset:///images/ic_add.png"))
        .onTriggered(this, SLOT(createEmptyView()))),
    _prevTabShortcut(Shortcut::create().key("q")
        .onTriggered(this, SLOT(setPrevTabActive()))),
    _nextTabShortcut(Shortcut::create().key("w")
        .onTriggered(this, SLOT(setNextTabActive()))),
    _nextProjectShortcut(Shortcut::create().key("n")
        .onTriggered(this, SLOT(setNextProjectActive()))),
    _changeProjectPathShortcut(Shortcut::create().key("o")
        .onTriggered(this, SLOT(changeProjectPath()))),
    _fpicker(NULL), _zoomed(false), _reopenSidebar(false),
    _enterKeyPressedOnTopScope(false)
{
    setShowTabsOnActionBar(false);
    addShortcut(_newViewShortcut);
    addShortcut(_prevTabShortcut);
    addShortcut(_nextTabShortcut);
    // project management
    addShortcut(_newProjectShortcut);
    addShortcut(_nextProjectShortcut);
    addShortcut(_changeProjectPathShortcut);
    addShortcut(Shortcut::create().key("Enter")
        .onTriggered(this, SLOT(flagEnterKeyOnTopScope())));
    addKeyListener(ModKeyListener::create(KEYCODE_RETURN, false)
        .onModifiedKeyPressed(this, SLOT(onModifiedKey(bb::cascades::KeyEvent*)))
        .onModKeyPressedAndReleased(this, SLOT(onModKey(bb::cascades::KeyEvent*))));

    // setup the projects
    // initial population of projects
    add(_newProjectControl);
    add(_newViewControl);
    for (int i = 0; i < _projects->size(); i++)
        onProjectInserted(i, _projects->at(i));
    conn(_projects, SIGNAL(itemInserted(int, Project*)),
        this, SLOT(onProjectInserted(int, Project*)));
    connectActiveProject(_projects->active(), false);
    conn(_projects, SIGNAL(activeItemChanged(Project*, Project*)),
        this, SLOT(onActiveProjectChanged(Project*, Project*)));
    conn(_projects, SIGNAL(itemRemoved(int, Project*)),
        this, SLOT(onProjectRemoved(int, Project*)));

    conn(this, SIGNAL(sidebarVisualStateChanged(bb::cascades::SidebarVisualState::Type)),
        this, SLOT(onSidebarVisualStateChanged(bb::cascades::SidebarVisualState::Type)));

    // load text
    onTranslatorChanged();
}

bool MultiViewPane::useNavigationPopup() const
{
    return !Helium::instance()->appearance()->fullScreen() &&
            sidebarVisualState() == bb::cascades::SidebarVisualState::Hidden;
}


void MultiViewPane::onModifiedKey(bb::cascades::KeyEvent *event)
{
    if (_enterKeyPressedOnTopScope) {
        _enterKeyPressedOnTopScope = false;
        if (event->keycap() == KEYCODE_BACKSPACE) {
            displayShortcuts();
        }
    }
}

void MultiViewPane::onModKey(bb::cascades::KeyEvent *)
{
    if (_enterKeyPressedOnTopScope) {
        _enterKeyPressedOnTopScope = false;
        // XXX: the only scenario where we know to require a
        // custom triggering is here...
        if (View *v = dynamic_cast<View *>(activeTab()))
            v->autoFocus();
    }
}

void MultiViewPane::flagEnterKeyOnTopScope()
{
    _enterKeyPressedOnTopScope = true;
}

void MultiViewPane::onProjectTriggered()
{
    Project *src = (Project *) sender();
    if (activeProject() == src)
        changeProjectPath();
    else {
        _projects->setActive(src);
        _reopenSidebar = true;
    }
}

void MultiViewPane::changeProjectPath()
{
    filePicker(activeProject()->path(), this,
            SLOT(onProjectPathSelected(const QStringList&)),
            SLOT(resetProjectActiveView()))->open();
}

void MultiViewPane::onProjectPathSelected(const QStringList &list)
{
    activeProject()->setPath(list[0]);
    resetProjectActiveView(useNavigationPopup());
}

void MultiViewPane::resetProjectActiveView(bool toast)
{
    setProjectActiveView(activeProject(),
            activeProject()->activeViewIndex(),
            activeProject()->activeView(), toast);
}

void MultiViewPane::setProjectActiveView(Project *project,
        int viewIndex, View *view, bool toast)
{
    setActiveTab(view);
    if (toast)
        Utility::toast(QString("%1/%2. %3\n[%4/%5. %6]")
                .arg(viewIndex+1)
                .arg(project->size())
                .arg(view->title())
                .arg(_projects->indexOf(project)+1)
                .arg(_projects->size())
                .arg(project->title()));
}

void MultiViewPane::connectActiveProject(Project *p, bool toast)
{
    p->select();
    for (int i = 0; i < p->size(); i++)
        add(p->at(i));
    resetProjectActiveView(toast);
    conn(p, SIGNAL(viewInserted(int, View*)),
        this, SLOT(onProjectViewInserted(int, View*)));
    conn(p, SIGNAL(viewRemoved(View*)),
        this, SLOT(onProjectViewRemoved(View*)));
    conn(p, SIGNAL(activeViewChanged(int, View*)),
        this, SLOT(onProjectActiveViewChanged(int, View*)));
}

void MultiViewPane::disconnectActiveProject(Project *p)
{
    p->unselect();
    disconn(p, SIGNAL(viewInserted(int, View*)),
        this, SLOT(onProjectViewInserted(int, View*)));
    disconn(p, SIGNAL(viewRemoved(View*)),
        this, SLOT(onProjectViewRemoved(View*)));
    disconn(p, SIGNAL(activeViewChanged(int, View*)),
        this, SLOT(onProjectActiveViewChanged(int, View*)));
    for (int i = 0; i < p->size(); i++)
        remove(p->at(i));
}

void MultiViewPane::onActiveProjectChanged(Project *change, Project *old)
{
    if (old)
        disconnectActiveProject(old);
    if (change)
        connectActiveProject(change, useNavigationPopup());
}

void MultiViewPane::setNextProjectActive()
{
    if (_projects->size() < 2)
        resetProjectActiveView(useNavigationPopup());
    else
        _projects->setActive(
                _projects->at(PMOD(_projects->indexOf(activeProject())+1, _projects->size())));
}

void MultiViewPane::resetProjectViewHeaderSubtitles()
{
    for (int i = 0, total = _projects->size(); i < total; i++)
        _projects->at(i)->resetViewHeaderSubtitles(i, total);
}

void MultiViewPane::onProjectInserted(int index, Project *project)
{
    insert(1+index, project);
    conn(this, SIGNAL(translatorChanged()),
        project, SIGNAL(translatorChanged()));
    conn(project, SIGNAL(triggered()),
        this, SLOT(onProjectTriggered()));
    resetProjectViewHeaderSubtitles();
}

void MultiViewPane::removeActiveProject()
{
    QStringList files;
    for (int i = 0; i < activeProject()->size(); i++) {
        if (activeProject()->at(i)->unsafeToRemove())
            files.append(QString("\t%1").arg(activeProject()->at(i)->title()));
    }
    if (!files.empty()) {
        Utility::dialog(tr("Yes"), tr("No"), tr("Unsaved change detected"),
                tr("These files have unsaved changes:\n"
                "\n%1\n\n"
                "Do you want to continue?").arg(files.join("\n")),
                this, SLOT(onUnsavedChangeDialogFinishedWhenClosingProject(bb::system::SystemUiResult::Type)));
    } else {
        _projects->removeAt(_projects->indexOf(activeProject()));
    }
}

void MultiViewPane::onUnsavedChangeDialogFinishedWhenClosingProject(bb::system::SystemUiResult::Type type)
{
    if (type == bb::system::SystemUiResult::ConfirmButtonSelection) {
        _projects->removeAt(_projects->indexOf(activeProject()));
    }
}

void MultiViewPane::onProjectRemoved(int index, Project *project)
{
    remove(project);
    if (!_projects->active())
        _projects->insert(0, new Project(_projects, Helium::instance()->general()->defaultProjectDirectory()));
    project->deleteLater();
    resetProjectViewHeaderSubtitles();
}

pickers::FilePicker *MultiViewPane::filePicker(const QString &directory,
            QObject *target,
            const char *onFileSelected, const char *onCancelled)
{
    if (!_fpicker) {
        _fpicker = new pickers::FilePicker(this);
        _fpicker->setMode(pickers::FilePickerMode::SaverMultiple);
    }
    _fpicker->setTitle(tr("Select Project Directory"));
    _fpicker->setDirectories(QStringList(directory));
    _fpicker->disconnect();
    conn(_fpicker, SIGNAL(fileSelected(const QStringList&)), target, onFileSelected);
    if (onCancelled) {
        conn(_fpicker, SIGNAL(canceled()), target, onCancelled);
    }
    return _fpicker;
}

void MultiViewPane::createProject()
{
    filePicker(Helium::instance()->general()->defaultProjectDirectory(), this,
            SLOT(onNewProjectPathSelected(const QStringList&)),
            SLOT(resetProjectActiveView()))->open();
}

void MultiViewPane::onNewProjectPathSelected(const QStringList &list)
{
    Project *p = new Project(_projects, list[0]);
    _projects->insert(_projects->indexOf(activeProject())+1, p);
    _projects->setActive(p);
}

void MultiViewPane::onProjectViewInserted(int index, View *view)
{
    insert(2+_projects->size()+index, view);
}

void MultiViewPane::onProjectViewRemoved(View *view)
{
    remove(view);
}

void MultiViewPane::onProjectActiveViewChanged(int index, View *view)
{
    setProjectActiveView(activeProject(), index, view, useNavigationPopup());
}

void MultiViewPane::zoomIntoView()
{
    if (!_zoomed) {
        _zoomed = true;
        while (count() > 0)
            remove(at(0));
        // set the active pane to use the current one
        setActivePane(activeProject()->activeView()->detachContent());
        _newViewShortcut->setEnabled(false);
        _newProjectShortcut->setEnabled(false);
        _nextProjectShortcut->setEnabled(false);
        _changeProjectPathShortcut->setEnabled(false);
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
        activeProject()->activeView()->reattachContent();
        add(_newProjectControl);
        for (int i = 0; i < _projects->size(); i++)
            add(_projects->at(i));
        // add views
        add(_newViewControl);
        // add the views in the projects back
        for (int i = 0; i < activeProject()->size(); i++)
            add(activeProject()->at(i));
        resetProjectActiveView(false);
        _newViewShortcut->setEnabled(true);
        _newProjectShortcut->setEnabled(true);
        _nextProjectShortcut->setEnabled(true);
        _changeProjectPathShortcut->setEnabled(true);
    }
}

void MultiViewPane::createEmptyView()
{
    activeProject()->createEmptyViewAt(activeProject()->activeViewIndex()+1);
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
    else if (activeProject()->size() < 2) // refresh display
        resetProjectActiveView(useNavigationPopup());
    else
        activeProject()->setActiveViewIndex(
                PMOD(activeProject()->activeViewIndex()+offset,
                        activeProject()->size()));
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
    emit translatorChanged();
}

void MultiViewPane::displayShortcuts()
{
    QList<ShortcutHelp> helps;
    Page *page = activePane()->top();
    if (dynamic_cast<View *>(page))
        helps << ShortcutHelp(QString(RETURN_SYMBOL), tr("Focus Editable Area"));
    for (int i = 0; i < page->shortcutCount(); i++)
        helps << ShortcutHelp::fromShortcut(page->shortcutAt(i));
    // ActionItems
    for (int i = 0; i < page->actionCount(); i++)
        helps << ShortcutHelp::fromActionItem(page->actionAt(i));
    // KeyListener
    for (int i = 0; i < page->keyListenerCount(); i++)
        helps << ShortcutHelp::fromKeyListener(page->keyListenerAt(i));
    helps << ShortcutHelp::fromPaneProperties(page->paneProperties())
          // from multiViewPane itself
          << ShortcutHelp::fromShortcut(_newProjectShortcut)
          << ShortcutHelp::fromShortcut(_newViewShortcut)
          << ShortcutHelp::fromShortcut(_prevTabShortcut)
          << ShortcutHelp::fromShortcut(_nextTabShortcut)
          << ShortcutHelp::fromShortcut(_nextProjectShortcut)
          << ShortcutHelp::fromShortcut(_changeProjectPathShortcut)
          << ShortcutHelp(BACKSPACE_SYMBOL, tr("Display Shortcuts"), QString(RETURN_SYMBOL));
    Utility::dialog(tr("Dismiss"), tr("Shortcuts"), ShortcutHelp::showAll(helps));
}
