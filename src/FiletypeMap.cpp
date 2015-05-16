/*
 * FiletypeMap.cpp
 *
 *  Created on: May 16, 2015
 *      Author: lingnan
 */

#include <QMutexLocker>
#include <srchilite/langmap.h>
#include <srchilite/instances.h>
#include <FiletypeMap.h>
#include <Filetype.h>
#include <Utility.h>

FiletypeMap::FiletypeMap()
{
    moveToThread(&_thread);
    conn(this, SIGNAL(initialize()), this, SLOT(openLangMap()));
    _thread.start();
    emit initialize();
}

FiletypeMap::~FiletypeMap()
{
    _thread.quit();
    _thread.wait();
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

void FiletypeMap::openLangMap()
{
    QMutexLocker lock(&_langMapMut);
    srchilite::Instances::getLangMap()->open();
}

Filetype *FiletypeMap::filetypeForName(const QString &filename)
{
    QMutexLocker lock(&_langMapMut);
    return filetype(QString::fromUtf8(
            srchilite::Instances::getLangMap()->getMappedFileNameFromFileName(
                filename.toUtf8().constData()).c_str()));
}
