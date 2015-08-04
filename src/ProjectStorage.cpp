/*
  * ProjectStorage.cpp
 *
 *  Created on: Jul 28, 2015
 *      Author: lingnan
 */

#include <ProjectStorage.h>
#include <Project.h>
#include <Utility.h>

ProjectStorage::ProjectStorage(const QString &prefix, QObject *parent):
    QObject(parent)
{
    _settings.beginGroup(prefix);
}

void ProjectStorage::setupProject(Project *project)
{
    conn(project, SIGNAL(pathChanged(const QString&)),
        this, SLOT(onProjectPathChanged(const QString&)));
}

void ProjectStorage::onActiveProjectChanged(Project *change, Project *old)
{
    if (old) {
        _settings.beginGroup(_idHash[old]);
        _settings.remove("active");
        _settings.endGroup();
    }
    if (change) {
        _settings.beginGroup(_idHash[change]);
        _settings.setValue("active", true);
        _settings.endGroup();
    }
}

void ProjectStorage::onProjectInserted(int index, Project *project)
{
    // add it into our id map
    QPair<Project *, uint> pair(project, 0);
    QString hash;
    uint i = 0;
    for (; i < UINT_MAX; i++) {
        qDebug() << "beginning try number" << i;
        pair.second = i;
        hash = QString::number(qHash(pair));
        qDebug() << "obtained hash" << hash;
        if (!_idSet.contains(hash)) {
            qDebug() << "using hash" << hash;
            _idSet.insert(hash);
            _idHash[project] = hash;
            setupProject(project);
            _settings.beginGroup(hash);
            _settings.setValue("path", project->path());
            _settings.setValue("index", index);
            _settings.endGroup();
            break;
        }
    }
    if (i == UINT_MAX)
        qWarning() << "HASH ERROR: unable to find a hash for project" << project;
    Zipper<Project *> *zipper = (Zipper<Project *> *) sender();
    // resave indices
    for (int i = index+1; i < zipper->size(); i++) {
        _settings.beginGroup(_idHash[zipper->at(i)]);
        _settings.setValue("index", i);
        _settings.endGroup();
    }
}

void ProjectStorage::onProjectRemoved(int index, Project *project)
{
    _settings.remove(_idHash[project]);
    Zipper<Project *> *zipper = (Zipper<Project *> *) sender();
    // resave indices
    for (int i = index; i < zipper->size(); i++) {
        _settings.beginGroup(_idHash[zipper->at(i)]);
        _settings.setValue("index", i);
        _settings.endGroup();
    }
}

void ProjectStorage::onProjectPathChanged(const QString &path)
{
    _settings.beginGroup(_idHash[(Project *) sender()]);
    _settings.setValue("path", path);
    _settings.endGroup();
}

Zipper<Project *> *ProjectStorage::read()
{
    Zipper<Project *> *zipper = new Zipper<Project *>(this);
    // iterates through the keys in the projects/
    QStringList keys = _settings.childGroups();
    if (keys.size() > 0) {
        qDebug() << "Reading projects...";
        QVector<Project *> vec(keys.size());
        Project *active = NULL;
        QStringList keys = _settings.childGroups();
        for (int i = 0; i < keys.size(); i++) {
            qDebug() << "Reading" << keys[i];
            _settings.beginGroup(keys[i]);
            Project *p = new Project(zipper, _settings.value("path").toString());
            vec[_settings.value("index").toInt()] = p;
            if (_settings.value("active").toBool()) {
                active = p;
            }
            // use the same hash
            Q_ASSERT(!_idSet.contains(keys[i]));
            _idSet.insert(keys[i]);
            _idHash[p] = keys[i];
            setupProject(p);
            _settings.endGroup();
        }
        for (int i = 0; i < vec.size(); i++) {
            Project *p = vec[i];
            if (p)
                zipper->append(p);
        }
        if (active)
            zipper->setActive(active);
    }
    conn(zipper, SIGNAL(activeItemChanged(Project*, Project*)),
        this, SLOT(onActiveProjectChanged(Project*, Project*)));
    conn(zipper, SIGNAL(itemInserted(int, Project*)),
        this, SLOT(onProjectInserted(int, Project*)));
    conn(zipper, SIGNAL(itemRemoved(int, Project*)),
        this, SLOT(onProjectRemoved(int, Project*)));
    if (zipper->empty()) {
        qDebug() << "Appending default project...";
        zipper->append(new Project(zipper, DEFAULT_PROJECT_PATH));
    }
    return zipper;
}

