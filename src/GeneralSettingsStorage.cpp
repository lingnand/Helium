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
    qDebug() << _settings.allKeys();
    _settings.beginGroup(prefix);
}

void GeneralSettingsStorage::onHighlightRangeChanged(int range)
{
    _settings.setValue("highlight_range", range);
}

GeneralSettings *GeneralSettingsStorage::read()
{
    QStringList keys = _settings.childKeys();
    GeneralSettings *settings;
    qDebug() << _settings.allKeys();
    if (keys.empty()) {
        qDebug() << "Populating the default general settings...";
        settings = new GeneralSettings(20, this);
        onHighlightRangeChanged(settings->highlightRange());
    } else {
        qDebug() << "Reading general settings...";
        settings = new GeneralSettings(
                _settings.value("highlight_range").toInt(),
                this);
    }
    conn(settings, SIGNAL(highlightRangeChanged(int)),
            this, SLOT(onHighlightRangeChanged(int)));
    return settings;
}
