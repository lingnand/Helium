/*
 * Project.cpp
 *
 *  Created on: Jul 25, 2015
 *      Author: lingnan
 */

#include <Project.h>
#include <Utility.h>
#include <View.h>
#include <Buffer.h>
#include <Helium.h>

using namespace bb::cascades;

Project::Project(const QString &path): _activeView(NULL)
{
    setPath(path);
    unselect();
    addNewViewAndSetActive();
}

Project::~Project()
{
    // this assumes that all the views should have been
    // removed from a MultiViewPane (which should be the case)
    for (int i = 0; i < size(); i++)
        at(i)->deleteLater();
}

void Project::setActiveViewIndex(int index)
{
    View *n = _views[index];
    if (n != _activeView) {
        _activeView = n;
        emit activeViewChanged(_activeView, false);
    }
}

void Project::onViewTriggered()
{
    View *src = (View *) sender();
    if (src != _activeView) {
        _activeView = src;
        emit activeViewChanged(_activeView, true);
    }
}

void Project::setPath(const QString &path)
{
    if (path != _path) {
        qDebug() << "setting path!!!" << path;
        _path = path;
        // shorten the path for display
        QString title = _path;
        if (_path.startsWith("/accounts/1000/removable/sdcard")) {
            QStringRef rem = _path.rightRef(_path.size()-31); // XXX: hardcoded length
            title = "SD";
            if (!rem.isEmpty()) {
                title += ":";
                title += rem;
            }
        } else if (_path.startsWith("/accounts/1000/shared/")) {
            title = _path.right(_path.size()-22); // XXX: hardcoded length
        }
        setTitle(title);
        emit pathChanged(_path);
    }
}

void Project::unselect()
{
    setImageSource(QUrl("asset:///images/ic_folder_unselected.png"));
}

void Project::select()
{
    setImageSource(QUrl("asset:///images/ic_folder_selected.png"));
}

void Project::insertNewView(int index, Buffer *buffer)
{
    View *view = new View(this, buffer);
    conn(this, SIGNAL(translatorChanged()), view, SLOT(onTranslatorChanged()));
    conn(view, SIGNAL(triggered()), this, SLOT(onViewTriggered()));
    _views.insert(index, view);
    setUnreadContentCount(_views.size());
    emit viewInserted(index, view);
}

void Project::addNewViewAndSetActive()
{
    int index = size();
    insertNewView(index, Helium::instance()->buffers()->newBuffer());
    setActiveViewIndex(index);
}

void Project::removeAt(int index)
{
    View *view = _views[index];
    _views.removeAt(index);
    setUnreadContentCount(_views.size());
    emit viewRemoved(view);
    if (_views.isEmpty()) {
        addNewViewAndSetActive();
    } else if (view == _activeView) {
        // activate the view after it (or before it if it's the last one)
        setActiveViewIndex(qMin(index, size()-1));
    }
    view->deleteLater();
}

void Project::cloneAt(int index)
{
    int i = index+1;
    insertNewView(i, _views[index]->buffer());
    setActiveViewIndex(i);
}
