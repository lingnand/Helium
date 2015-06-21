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

GeneralSettings *GeneralSettingsStorage::read()
{
    QStringList keys = _settings.childKeys();
    GeneralSettings *settings;
    if (keys.empty()) {
        qDebug() << "Populating the default general settings...";
        settings = new GeneralSettings(23, "/accounts/1000/shared", this);
        onHighlightRangeChanged(settings->highlightRange());
        onDefaultOpenDirectoryChanged(settings->defaultOpenDirectory());
    } else {
        qDebug() << "Reading general settings...";
        settings = new GeneralSettings(
                _settings.value("highlight_range").toInt(),
                _settings.value("default_open_directory").toString(),
                this);
    }
    conn(settings, SIGNAL(highlightRangeChanged(int)),
            this, SLOT(onHighlightRangeChanged(int)));
    conn(settings, SIGNAL(defaultOpenDirectoryChanged(const QString&)),
            this, SLOT(onDefaultOpenDirectoryChanged(const QString&)));
    return settings;
}
