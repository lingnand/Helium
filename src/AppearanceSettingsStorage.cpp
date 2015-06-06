/*
 * AppearanceSettingsStorage.cpp
 *
 *  Created on: Jun 6, 2015
 *      Author: lingnan
 */

#include <QDebug>
#include <QStringList>
#include <AppearanceSettingsStorage.h>
#include <AppearanceSettings.h>
#include <Utility.h>

AppearanceSettingsStorage::AppearanceSettingsStorage(const QString &prefix, QObject *parent):
    QObject(parent)
{
    _settings.beginGroup(prefix);
}

void AppearanceSettingsStorage::onHideActionBarChanged(bool hide)
{
    _settings.setValue("hide_actionbar", hide);
}

AppearanceSettings *AppearanceSettingsStorage::read()
{
    QStringList keys = _settings.childKeys();
    AppearanceSettings *settings;
    if (keys.empty()) {
        qDebug() << "Populating the default appearance settings...";
        settings = new AppearanceSettings(false, this);
        onHideActionBarChanged(settings->hideActionBar());
    } else {
        qDebug() << "Reading appearance settings...";
        settings = new AppearanceSettings(
                _settings.value("hide_actionbar").toBool(),
                this);
    }
    conn(settings, SIGNAL(hideActionBarChanged(bool)),
            this, SLOT(onHideActionBarChanged(bool)));
    return settings;

}
