/*
 * AppearanceSettingsStorage.h
 *
 *  Created on: Jun 6, 2015
 *      Author: lingnan
 */

#ifndef APPEARANCESETTINGSSTORAGE_H_
#define APPEARANCESETTINGSSTORAGE_H_

#include <QSettings>
#include <bb/cascades/VisualStyle>
#include <AppearanceSettings.h>

class AppearanceSettingsStorage : public QObject
{
    Q_OBJECT
public:
    AppearanceSettingsStorage(const QString &prefix, QObject *parent=NULL);
    AppearanceSettings *read();
private:
    QSettings _settings;
    Q_SLOT void onHideActionBarChanged(bool);
    Q_SLOT void onThemeChanged(AppearanceSettings::Theme);
};

#endif /* APPEARANCESETTINGSSTORAGE_H_ */
