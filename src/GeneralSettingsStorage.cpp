/*
 * GeneralSettingsStorage.cpp
 *
 *  Created on: May 25, 2015
 *      Author: lingnan
 */

#include <QStringList>
#include <QDebug>
#include <bb/ApplicationInfo>
#include <GeneralSettingsStorage.h>
#include <GeneralSettings.h>
#include <Utility.h>

GeneralSettingsStorage::GeneralSettingsStorage(const QString &prefix, QObject *parent):
    QObject(parent)
{
    _settings.beginGroup(prefix);
}

void GeneralSettingsStorage::onHighlightRangeChanged(int range)
{
    _settings.setValue("highlight_range", range);
}

void GeneralSettingsStorage::onDefaultProjectDirectoryChanged(const QString &dir)
{
    _settings.setValue("default_project_directory", dir);
}

void GeneralSettingsStorage::onSupportConfirmed()
{
    _settings.setValue("has_confirmed_support", true);
}

GeneralSettings *GeneralSettingsStorage::read()
{
    qDebug() << "Reading general settings...";
    GeneralSettings *settings = new GeneralSettings(
        _settings.value("highlight_range", 23).toInt(),
        _settings.value("default_project_directory", DEFAULT_PROJECT_PATH).toString(),
        _settings.value("number_of_times_launched", 0).toInt() + 1,
        Version(_settings.value("last_version").toString()),
        Version(bb::ApplicationInfo().version()),
        _settings.value("has_confirmed_support", false).toBool(),
        this);
    _settings.setValue("number_of_times_launched", settings->numberOfTimesLaunched());
    _settings.setValue("last_version", settings->currentVerison().string());
    conn(settings, SIGNAL(highlightRangeChanged(int)),
            this, SLOT(onHighlightRangeChanged(int)));
    conn(settings, SIGNAL(defaultProjectDirectoryChanged(const QString&)),
            this, SLOT(onDefaultProjectDirectoryChanged(const QString&)));
    conn(settings, SIGNAL(confirmedSupport()),
            this, SLOT(onSupportConfirmed()));
    return settings;
}
