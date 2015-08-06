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

Project::Project(Zipper<Project *> *projects, const QString &path):
        _projects(projects),
        _activeView(NULL)
{
    setPath(path);
    unselect();
    createEmptyViewAt(0);
}

Project::~Project()
{
    // this assumes that all the views should have been
    // removed from a MultiViewPane (which should be the case)
    for (int i = 0; i < _views.size(); i++)
        _views[i]->deleteLater();
}

bool Project::setActiveViewIndex(int index)
{
    View *n = _views[index];
    if (n != _activeView) {
        if (_activeView)
            _activeView->onDeactivated();
        _activeView = n;
        _activeView->onActivated();
        emit activeViewChanged(index, _activeView);
        return true;
    }
    return false;
}

void Project::onViewTriggered()
{
    View *src = (View *) sender();
    if (src != _activeView) {
        _activeView = src;
        emit activeViewChanged(activeViewIndex(), _activeView);
    }
}

void Project::setPath(const QString &path)
{
    if (path != _path) {
        _path = path;
        setTitle(Utility::shortenPath(_path));
        resetViewHeaderSubtitles();
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
    _views.insert(index, view);
    conn(view, SIGNAL(triggered()), this, SLOT(onViewTriggered()));
    conn(this, SIGNAL(translatorChanged()), view, SLOT(onTranslatorChanged()));
    // title
    resetViewHeaderTitles();
    conn(view, SIGNAL(titleChanged(const QString&)), view, SLOT(resetHeaderTitle()));
    // subtitle
    view->resetHeaderSubtitle(_projects->indexOf(this), _projects->size());
    setUnreadContentCount(_views.size());
    emit viewInserted(index, view);
}

void Project::resetViewHeaderSubtitles()
{
    resetViewHeaderSubtitles(_projects->indexOf(this), _projects->size());
}

void Project::resetViewHeaderSubtitles(int index, int total)
{
    for (int i = 0; i < _views.size(); i++)
        _views[i]->resetHeaderSubtitle(index, total);
}

void Project::resetViewHeaderTitles()
{
    for (int i = 0, total = _views.size(); i < total; i++)
        _views[i]->resetHeaderTitle(i, total);
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
    resetViewHeaderTitles();
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
                emit activeViewChanged(toActivate, _activeView);
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
        emit activeViewChanged(index+i, view);
    }
}
