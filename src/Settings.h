/*
 * Settings.h
 *
 *  Created on: May 24, 2015
 *      Author: lingnan
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <bb/cascades/Page>
#include <bb/cascades/QListDataModel>

namespace bb {
    namespace cascades {
        class TitleBar;
    }
}

class GeneralSettings;

// the Settings UI for the entire app
class Settings : public bb::cascades::Page
{
    Q_OBJECT
public:
    Settings();
    virtual ~Settings() {}
    Q_SLOT void onTranslatorChanged();
Q_SIGNALS:
    void pageSelected(bb::cascades::Page *);
private:
    bb::cascades::TitleBar *_title;
    bb::cascades::QListDataModel<QString> _model;

    GeneralSettings *_generalSettings;
    Q_SLOT void onTriggered(QVariantList);
};

#endif /* SETTINGS_H_ */
