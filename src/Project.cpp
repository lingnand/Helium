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
#include <BufferStore.h>

using namespace bb::cascades;

Project::Project(const QString &path): _activeView(NULL)
{
    setPath(path);
    unselect();
    createEmptyViewAt(0);
}

Project::~Project()
{
    // this assumes that all the views should have been
    // removed from a MultiViewPane (which should be the case)
    for (int i = 0; i < size(); i++)
        _views[i]->deleteLater();
}

bool Project::setActiveViewIndex(int index)
{
    View *n = _views[index];
    if (n != _activeView) {
        if (_activeView)
            _activeView->onOutOfView();
        _activeView = n;
        emit activeViewChanged(index, _activeView, false);
        return true;
    }
    return false;
}

void Project::onViewTriggered()
{
    View *src = (View *) sender();
    if (src != _activeView) {
        _activeView = src;
        emit activeViewChanged(activeViewIndex(), _activeView, true);
    }
}

void Project::setPath(const QString &path)
{
    if (path != _path) {
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

void Project::createEmptyViewAt(int index)
{
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
        createEmptyViewAt(0);
    } else if (view == _activeView) {
        // activate the view before it (or after it if it's the first one)
        setActiveViewIndex(qMax(index-1, 0));
    }
    view->deleteLater();
}

void Project::cloneAt(int index)
{
    int i = index+1;
    insertNewView(i, _views[index]->buffer());
    setActiveViewIndex(i);
}

void Project::openFilesAt(int index, const QStringList &files)
{
    BufferStore *buffers = Helium::instance()->buffers();
    Buffer *b;
    View *view = _views[index];
    if (!view->untouched()) {
        // if one of the files has the same filepath as view
        // then we don't open a new view
        int offset = 1;
        for (int i = files.size()-1; i >= 0; i--) {
            if (files[i] == view->buffer()->filepath()) {
                offset = 0;
                continue;
            }
            b = buffers->bufferForFilepath(files[i]);
            if (!b) {
                b = buffers->newBuffer();
                b->load(files[i]);
            }
            insertNewView(index+offset, b);
        }
        // activate the last inserted view
        int toActivate = index+offset+files.size()-1;
        if (!setActiveViewIndex(toActivate)) {
            if (files.size() > 1) // index DID change
                emit activeViewChanged(toActivate, _activeView, false);
        }
        return;
    }
    int i = 0;
    for (; i < files.size()-1; i++) {
        b = buffers->bufferForFilepath(files[i]);
        if (!b) {
            b = buffers->newBuffer();
            b->load(files[i]);
        }
        insertNewView(index+i, b);
    }
    if (files[i] != view->buffer()->filepath()) {
        b = buffers->bufferForFilepath(files[i]);
        if (b) {
            view->setBuffer(b);
        } else {
            if (view->buffer()->views().size() != 1) {
                // not only bound to this view!
                view->setBuffer(buffers->newBuffer());
            }
            view->buffer()->load(files[i]);
        }
    }
    if (i > 0) {
        emit activeViewChanged(index+i, view, false);
    }
}
