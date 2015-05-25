/*
 * GeneralSettings.h
 *
 *  Created on: May 25, 2015
 *      Author: lingnan
 */

#ifndef GENERALSETTINGS_H_
#define GENERALSETTINGS_H_

#include <bb/cascades/Page>

namespace bb {
    namespace cascades {
        class TitleBar;
        class Label;
    }
}

class HighlightRangePicker;

class GeneralSettings : public bb::cascades::Page
{
    Q_OBJECT
public:
    GeneralSettings();
    virtual ~GeneralSettings() {}
    Q_SLOT void onTranslatorChanged();
Q_SIGNALS:
    void highlightRangeChanged(int);
private:
    bb::cascades::TitleBar *_title;
    bb::cascades::Label *_help;
    HighlightRangePicker *_picker;
};

#endif /* GENERALSETTINGS_H_ */
