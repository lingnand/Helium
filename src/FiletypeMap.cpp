/*
 * FiletypeMap.cpp
 *
 *  Created on: May 16, 2015
 *      Author: lingnan
 */

#include <QMutexLocker>
#include <FiletypeMap.h>
#include <Filetype.h>
#include <Utility.h>

FiletypeMap::FiletypeMap(QObject *parent):
    QObject(parent),
    _langMapOpener(_langMapMut)
{
    _langMapOpener.start();
}

void FiletypeMap::add(Filetype *filetype)
{
    _filetypeMap[filetype->name()] = filetype;
}

FiletypeMap &FiletypeMap::operator<<(Filetype *filetype)
{
    add(filetype);
    return *this;
}

Filetype *FiletypeMap::filetype(const QString &filetypeName) const
{
    return _filetypeMap.value(filetypeName);
}

Filetype *FiletypeMap::filetypeForName(const QString &filename)
{
    QMutexLocker lock(&_langMapMut);
    return filetype(QString::fromStdString(
            srchilite::Instances::getLangMap()->getMappedFileNameFromFileName(
                filename.toStdString())));
}

QList<Filetype *> FiletypeMap::filetypes() const
{
    return _filetypeMap.values();
}
