/*
 * FiletypeMap.h
 *
 *  Created on: May 16, 2015
 *      Author: lingnan
 */

#ifndef FILETYPEMAP_H_
#define FILETYPEMAP_H_

#include <QMutex>
#include <QThread>
#include <srchilite/langmap.h>
#include <srchilite/instances.h>
#include <HighlightType.h>

class FiletypeMap : public QObject
{
    Q_OBJECT
public:
    FiletypeMap(QObject *parent=NULL);
    void add(Filetype *);
    FiletypeMap &operator<<(Filetype *);
    Filetype *filetype(const QString &filetypeName) const;
    Filetype *filetypeForName(const QString &filename);
    QList<Filetype *> filetypes() const;
private:
    class LangMapOpener : public QThread {
        QMutex &_mut;
    public:
        LangMapOpener(QMutex &mut): _mut(mut) {}
        void run() {
            QMutexLocker lock(&_mut);
            srchilite::Instances::getLangMap()->open();
        }
    } _langMapOpener;
    QMutex _langMapMut;
    QMap<QString, Filetype *> _filetypeMap;
};

#endif /* FILETYPEMAP_H_ */
