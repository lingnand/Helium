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

void AppearanceSettingsStorage::onHideTitleBarChanged(bool hide)
{
    _settings.setValue("hide_titlebar", hide);
}

void AppearanceSettingsStorage::onFullScreenChanged(bool full)
{
    _settings.setValue("full_screen", full);
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
    qDebug() << "Reading appearance settings...";
    AppearanceSettings *settings = new AppearanceSettings(
        _settings.value("hide_actionbar", true).toBool(),
        _settings.value("hide_titlebar", false).toBool(),
        _settings.value("full_screen", false).toBool(),
        (AppearanceSettings::Theme) _settings.value("theme", AppearanceSettings::SummerFruit).toInt(),
        _settings.value("font_family", QString()).toString(),
        (FontSize::Type) _settings.value("font_size", FontSize::Default).toInt(),
        this);
    conn(settings, SIGNAL(hideActionBarChanged(bool)),
            this, SLOT(onHideActionBarChanged(bool)));
    conn(settings, SIGNAL(hideTitleBarChanged(bool)),
            this, SLOT(onHideTitleBarChanged(bool)));
    conn(settings, SIGNAL(fullScreenChanged(bool)),
            this, SLOT(onFullScreenChanged(bool)));
    conn(settings, SIGNAL(themeChanged(AppearanceSettings::Theme)),
            this, SLOT(onThemeChanged(AppearanceSettings::Theme)));
    conn(settings, SIGNAL(fontFamilyChanged(const QString&)),
            this, SLOT(onFontFamilyChanged(const QString&)));
    conn(settings, SIGNAL(fontSizeChanged(bb::cascades::FontSize::Type)),
            this, SLOT(onFontSizeChanged(bb::cascades::FontSize::Type)));
    return settings;

}
