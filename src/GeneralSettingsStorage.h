/*
 * GeneralSettingsStorage.h
 *
 *  Created on: May 25, 2015
 *      Author: lingnan
 */

#ifndef GENERALSETTINGSSTORAGE_H_
#define GENERALSETTINGSSTORAGE_H_

#include <QSettings>

class GeneralSettings;

class GeneralSettingsStorage : public QObject
{
    Q_OBJECT
public:
    GeneralSettingsStorage(const QString &prefix, QObject *parent=NULL);
    GeneralSettings *read();
private:
    QSettings _settings;
    Q_SLOT void onHighlightRangeChanged(int);
    Q_SLOT void onDefaultOpenDirectoryChanged(const QString &);
};

#endif /* GENERALSETTINGSSTORAGE_H_ */
