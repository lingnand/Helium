/*
 * ProjectStorage.h
 *
 *  Created on: Jul 28, 2015
 *      Author: lingnan
 */

#ifndef PROJECTSTORAGE_H_
#define PROJECTSTORAGE_H_

#include <QSettings>
#include <QSet>
#include <Zipper.h>

class Project;

class ProjectStorage : public QObject
{
    Q_OBJECT
public:
    ProjectStorage(const QString &prefix, QObject *parent=NULL);
    Zipper<Project *> *read();
private:
    QSettings _settings;
    QHash<Project *, QString> _idHash; // from project to hashes
    QSet<QString> _idSet; // used ids
    void setupProject(Project *);
    Q_SLOT void onActiveProjectChanged(Project *change, Project *old);
    Q_SLOT void onProjectInserted(int, Project *);
    Q_SLOT void onProjectRemoved(int, Project *);
    Q_SLOT void onProjectPathChanged(const QString &);
};

#endif /* PROJECTSTORAGE_H_ */
