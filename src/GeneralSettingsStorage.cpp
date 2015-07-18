/*
 * GeneralSettingsStorage.cpp
 *
 *  Created on: May 25, 2015
 *      Author: lingnan
 */

#include <QStringList>
#include <QDebug>
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

void GeneralSettingsStorage::onDefaultOpenDirectoryChanged(const QString &dir)
{
    _settings.setValue("default_open_directory", dir);
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
        _settings.value("default_open_directory", "/accounts/1000/shared").toString(),
        _settings.value("number_of_times_launched", 0).toInt() + 1,
        _settings.value("has_confirmed_support", false).toBool(),
        this);
    _settings.setValue("number_of_times_launched", settings->numberOfTimesLaunched());
    conn(settings, SIGNAL(highlightRangeChanged(int)),
            this, SLOT(onHighlightRangeChanged(int)));
    conn(settings, SIGNAL(defaultOpenDirectoryChanged(const QString&)),
            this, SLOT(onDefaultOpenDirectoryChanged(const QString&)));
    conn(settings, SIGNAL(confirmedSupport()),
            this, SLOT(onSupportConfirmed()));
    return settings;
}
