/*
 * AppearanceSettingsStorage.cpp
 *
 *  Created on: Jun 6, 2015
 *      Author: lingnan
 */

#include <QDebug>
#include <QStringList>
#include <AppearanceSettingsStorage.h>
#include <Utility.h>

using namespace bb::cascades;

AppearanceSettingsStorage::AppearanceSettingsStorage(const QString &prefix, QObject *parent):
    QObject(parent)
{
    _settings.beginGroup(prefix);
}

void AppearanceSettingsStorage::onHideActionBarChanged(bool hide)
{
    _settings.setValue("hide_actionbar", hide);
}

Q_SLOT void AppearanceSettingsStorage::onThemeChanged(AppearanceSettings::Theme theme)
{
    _settings.setValue("theme", theme);
}

AppearanceSettings *AppearanceSettingsStorage::read()
{
    QStringList keys = _settings.childKeys();
    AppearanceSettings *settings;
    if (keys.empty()) {
        qDebug() << "Populating the default appearance settings...";
        settings = new AppearanceSettings(false, AppearanceSettings::Bright, this);
        onHideActionBarChanged(settings->hideActionBar());
        onThemeChanged(settings->theme());
    } else {
        qDebug() << "Reading appearance settings...";
        settings = new AppearanceSettings(
                _settings.value("hide_actionbar").toBool(),
                (AppearanceSettings::Theme) _settings.value("theme").toInt(),
                this);
    }
    conn(settings, SIGNAL(hideActionBarChanged(bool)),
            this, SLOT(onHideActionBarChanged(bool)));
    conn(settings, SIGNAL(themeChanged(AppearanceSettings::Theme)),
            this, SLOT(onThemeChanged(AppearanceSettings::Theme)));
    return settings;

}
