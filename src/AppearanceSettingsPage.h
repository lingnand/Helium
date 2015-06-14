/*
 * AppearanceSettingsPage.h
 *
 *  Created on: Jun 6, 2015
 *      Author: lingnan
 */

#ifndef APPEARANCESETTINGSPAGE_H_
#define APPEARANCESETTINGSPAGE_H_

namespace bb {
    namespace cascades {
        class Label;
        class ToggleButton;
        class DropDown;
        class Option;
        class Header;
    }
}

class NumberPicker;

#include <bb/cascades/FontSize>
#include <RepushablePage.h>
#include <AppearanceSettings.h>

class AppearanceSettingsPage : public RepushablePage
{
    Q_OBJECT
public:
    AppearanceSettingsPage(AppearanceSettings *, QObject *parent=NULL);
    Q_SLOT void onTranslatorChanged();
private:
    AppearanceSettings *_settings;
    bb::cascades::ToggleButton *_hideActionBarToggle;
    bb::cascades::Label *_hideActionBarToggleLabel;
    bb::cascades::Label *_hideActionBarHelp;
    bb::cascades::DropDown *_themeSelect;
    bb::cascades::Label *_themeSelectHelp;
    bb::cascades::Header *_fontHeader;
    bb::cascades::DropDown *_fontFamilySelect;
    bb::cascades::Option *_defaultFontFamilyOption;
    bb::cascades::DropDown *_fontSizeSelect;
    bb::cascades::Option *_defaultFontSizeOption;
    bb::cascades::Option *_extraSmallFontSizeOption;
    bb::cascades::Option *_smallFontSizeOption;
    bb::cascades::Option *_mediumFontSizeOption;
    bb::cascades::Option *_largeFontSizeOption;
    bb::cascades::Option *_extraLargeFontSizeOption;
    Q_SLOT void onThemeChanged(AppearanceSettings::Theme);
    Q_SLOT void onThemeSelectionChanged(const QVariant &);
    Q_SLOT void onFontFamilyChanged(const QString &);
    Q_SLOT void onFontFamilySelectionChanged(const QVariant&);
    Q_SLOT void onFontSizeChanged(bb::cascades::FontSize::Type);
    Q_SLOT void onFontSizeSelectionChanged(const QVariant&);
};

#endif /* APPEARANCESETTINGSPAGE_H_ */
