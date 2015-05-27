/*
 * SettingsPage.h
 *
 *  Created on: May 24, 2015
 *      Author: lingnan
 */

#ifndef SETTINGSPAGE_H_
#define SETTINGSPAGE_H_

#include <bb/cascades/Page>
#include <bb/cascades/QListDataModel>
#include <RepushablePage.h>

class GeneralSettings;
class GeneralSettingsPage;
class FiletypeMap;
class FiletypeMapSettingsPage;

// the Settings Page for the entire app
class SettingsPage : public RepushablePage
{
    Q_OBJECT
public:
    SettingsPage(GeneralSettings *, FiletypeMap *, QObject *parent=NULL);
    Q_SLOT void onTranslatorChanged();
Q_SIGNALS:
    void toPush(bb::cascades::Page *);
private:
    bb::cascades::QListDataModel<QString> _model;

    GeneralSettings *_generalSettings;
    GeneralSettingsPage *_generalSettingsPage;

    FiletypeMap *_filetypeMap;
    FiletypeMapSettingsPage *_filetypeMapSettingsPage;
    Q_SLOT void onTriggered(QVariantList);
};

#endif /* SETTINGSPAGE_H_ */
