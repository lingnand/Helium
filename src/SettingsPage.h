/*
 * SettingsPage.h
 *
 *  Created on: May 24, 2015
 *      Author: lingnan
 */

#ifndef SETTINGSPAGE_H_
#define SETTINGSPAGE_H_

#include <bb/cascades/NavigationPane>
#include <bb/cascades/QListDataModel>

namespace bb {
    namespace cascades {
        class Page;
    }
}

class GeneralSettings;
class GeneralSettingsPage;
class AppearanceSettings;
class AppearanceSettingsPage;
class FiletypeMap;
class FiletypeMapSettingsPage;

// the Settings Page for the entire app
class SettingsPage : public bb::cascades::NavigationPane
{
    Q_OBJECT
public:
    SettingsPage(GeneralSettings *, AppearanceSettings *, FiletypeMap *);
    Q_SLOT void onTranslatorChanged();
private:
    GeneralSettings *_generalSettings;
    GeneralSettingsPage *_generalSettingsPage;

    AppearanceSettings *_appearanceSettings;
    AppearanceSettingsPage *_appearanceSettingsPage;

    FiletypeMap *_filetypeMap;
    FiletypeMapSettingsPage *_filetypeMapSettingsPage;

    bb::cascades::Page *_base;
    bb::cascades::QListDataModel<QString> _model;

    Q_SLOT void closeSheet();
    Q_SLOT void onTriggered(QVariantList);
};

#endif /* SETTINGSPAGE_H_ */
