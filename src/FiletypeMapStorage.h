/*
 * FiletypeMapStorage.h
 *
 *  Created on: May 17, 2015
 *      Author: lingnan
 */

#ifndef FILETYPEMAPSTORAGE_H_
#define FILETYPEMAPSTORAGE_H_

#include <QSettings>

class Filetype;
class FiletypeMap;
class RunProfileManager;
class CmdRunProfileManager;

class FiletypeMapStorage : public QObject
{
    Q_OBJECT
public:
    FiletypeMapStorage(const QString &prefix, QObject *parent=NULL);
    FiletypeMap *read(); // read the map from settings, or create a default one
private:
    QSettings _settings;
    void insertRunProfileManager(RunProfileManager *);
    void insertFiletype(Filetype *);
    void connectFiletype(Filetype *);
    void connectCmdRunProfileManager(CmdRunProfileManager *);
    Q_SLOT void onCmdRunProfileManagerCmdChanged(const QString &);
    Q_SLOT void onFiletypeRunProfileManagerChanged(RunProfileManager *from, RunProfileManager *to);
    Q_SLOT void onFiletypeHighlightEnabledChanged(bool);
};


#endif /* FILETYPEMAPSTORAGE_H_ */
