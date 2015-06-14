/*
 * GeneralSettingsPage.h
 *
 *  Created on: May 25, 2015
 *      Author: lingnan
 */

#ifndef GENERALSETTINGSPAGE_H_
#define GENERALSETTINGSPAGE_H_

#include <RepushablePage.h>

namespace bb {
    namespace cascades {
        class Label;
    }
}

class GeneralSettings;
class NumberPicker;

class GeneralSettingsPage : public RepushablePage
{
    Q_OBJECT
public:
    GeneralSettingsPage(GeneralSettings *, QObject *parent=NULL);
    Q_SLOT void onTranslatorChanged();
private:
    bb::cascades::Label *_help;
    NumberPicker *_picker;
};

#endif /* GENERALSETTINGSPAGE_H_ */
