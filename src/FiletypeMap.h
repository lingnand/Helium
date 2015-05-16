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

class Filetype;

class FiletypeMap : public QObject
{
    Q_OBJECT
public:
    FiletypeMap();
    virtual ~FiletypeMap();
    void add(Filetype *);
    FiletypeMap &operator<<(Filetype *);
    Filetype *filetype() const;
    Filetype *filetype(const QString &filetypeName) const;
    Filetype *filetypeForName(const QString &filename);
Q_SIGNALS:
    void initialize();
private:
    QThread _thread;
    QMutex _langMapMut;
    QMap<QString, Filetype *> _filetypeMap;
    Q_SLOT void openLangMap();
};

#endif /* FILETYPEMAP_H_ */
