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
    }
}

#include <RepushablePage.h>

class AppearanceSettings;

class AppearanceSettingsPage : public RepushablePage
{
    Q_OBJECT
public:
    AppearanceSettingsPage(AppearanceSettings *, QObject *parent=NULL);
    Q_SLOT void onTranslatorChanged();
private:
    bb::cascades::ToggleButton *_hideActionBarToggle;
    bb::cascades::Label *_hideActionBarToggleLabel;
    bb::cascades::Label *_hideActionBarHelp;
};

#endif /* APPEARANCESETTINGSPAGE_H_ */
