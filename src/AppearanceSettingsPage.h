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
    }
}

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
    bb::cascades::Option *_brightThemeOption;
    bb::cascades::Option *_darkThemeOption;
    bb::cascades::Label *_themeSelectHelp;
    Q_SLOT void onThemeChanged(AppearanceSettings::Theme);
    Q_SLOT void onThemeSelectionChanged(const QVariant &);
};

#endif /* APPEARANCESETTINGSPAGE_H_ */
