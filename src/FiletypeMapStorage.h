/*
 * FiletypeMapStorage.h
 *
 *  Created on: May 17, 2015
 *      Author: lingnan
 */

#ifndef FILETYPEMAPSTORAGE_H_
#define FILETYPEMAPSTORAGE_H_

#include <QSettings>
#include <WebRunProfileManager.h>

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
    void connectWebRunProfileManager(WebRunProfileManager *);
    Q_SLOT void onCmdRunProfileManagerCmdChanged(const QString &);
    Q_SLOT void onWebRunProfileManagerModeChanged(WebRunProfile::Mode);
    Q_SLOT void onFiletypeRunProfileManagerChanged(RunProfileManager *change, RunProfileManager *old);
    Q_SLOT void onFiletypeHighlightEnabledChanged(bool);
    Q_SLOT void onFiletypeTabSpaceConversionEnabledChanged(bool);
    Q_SLOT void onFiletypeNumberOfSpacesForTabChanged(int);
};


#endif /* FILETYPEMAPSTORAGE_H_ */
