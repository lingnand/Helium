/*
 * GeneralSettingsPage.h
 *
 *  Created on: May 25, 2015
 *      Author: lingnan
 */

#ifndef GENERALSETTINGSPAGE_H_
#define GENERALSETTINGSPAGE_H_

#include <bb/cascades/Page>
#include <RepushablePage.h>

namespace bb {
    namespace cascades {
        class TitleBar;
        class Label;
    }
}

class GeneralSettings;
class HighlightRangePicker;

class GeneralSettingsPage : public RepushablePage
{
    Q_OBJECT
public:
    GeneralSettingsPage(GeneralSettings *, QObject *parent=NULL);
    Q_SLOT void onTranslatorChanged();
private:
    bb::cascades::TitleBar *_title;
    bb::cascades::Label *_help;
    HighlightRangePicker *_picker;
    Q_SLOT void onHighlightRangeChanged(int);
};

#endif /* GENERALSETTINGSPAGE_H_ */
