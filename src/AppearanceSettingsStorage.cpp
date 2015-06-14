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

void AppearanceSettingsStorage::onThemeChanged(AppearanceSettings::Theme theme)
{
    _settings.setValue("theme", theme);
}

void AppearanceSettingsStorage::onFontFamilyChanged(const QString &fontFamily)
{
    _settings.setValue("font_family", fontFamily);
}

void AppearanceSettingsStorage::onFontSizeChanged(FontSize::Type fontSize)
{
    _settings.setValue("font_size", fontSize);
}

AppearanceSettings *AppearanceSettingsStorage::read()
{
    QStringList keys = _settings.childKeys();
    AppearanceSettings *settings;
    if (keys.empty()) {
        qDebug() << "Populating the default appearance settings...";
        settings = new AppearanceSettings(false, AppearanceSettings::SummerFruit,
                QString(), FontSize::Default,
                this);
        onHideActionBarChanged(settings->hideActionBar());
        onThemeChanged(settings->theme());
        onFontFamilyChanged(settings->fontFamily());
        onFontSizeChanged(settings->fontSize());
    } else {
        qDebug() << "Reading appearance settings...";
        settings = new AppearanceSettings(
                _settings.value("hide_actionbar").toBool(),
                (AppearanceSettings::Theme) _settings.value("theme").toInt(),
                _settings.value("font_family").toString(),
                (FontSize::Type) _settings.value("font_size").toInt(),
                this);
    }
    conn(settings, SIGNAL(hideActionBarChanged(bool)),
            this, SLOT(onHideActionBarChanged(bool)));
    conn(settings, SIGNAL(themeChanged(AppearanceSettings::Theme)),
            this, SLOT(onThemeChanged(AppearanceSettings::Theme)));
    conn(settings, SIGNAL(fontFamilyChanged(const QString&)),
            this, SLOT(onFontFamilyChanged(const QString&)));
    conn(settings, SIGNAL(fontSizeChanged(bb::cascades::FontSize::Type)),
            this, SLOT(onFontSizeChanged(bb::cascades::FontSize::Type)));
    return settings;

}
