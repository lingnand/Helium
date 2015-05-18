/*
 * FiletypeMapSettings.h
 *
 *  Created on: May 17, 2015
 *      Author: lingnan
 */

#ifndef FILETYPEMAPSETTINGS_H_
#define FILETYPEMAPSETTINGS_H_

#include <QSettings>

class Filetype;
class FiletypeMap;
class RunProfileManager;

class FiletypeMapSettings : public QObject
{
    Q_OBJECT
public:
    FiletypeMapSettings(const QString &prefix, QObject *parent=NULL);
    FiletypeMap *read(); // read the map from settings, or create a default one
private:
    QSettings _settings;
    void insertRunProfileManager(RunProfileManager *);
    void insertFiletype(Filetype *);
    void connectFiletype(Filetype *filetype);
    Q_SLOT void onCmdRunProfileManagerCmdChanged(const QString &);
    Q_SLOT void onFiletypeRunProfileManagerChanged(RunProfileManager *from, RunProfileManager *to);
    Q_SLOT void onFiletypeHighlightEnabledChanged(bool);
};


#endif /* FILETYPEMAPSETTINGS_H_ */
