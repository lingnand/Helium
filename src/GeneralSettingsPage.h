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
        class DropDown;
        class Option;
        namespace pickers {
            class FilePicker;
        }
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
    GeneralSettings *_settings;
    NumberPicker *_highlightRangePicker;
    bb::cascades::Label *_highlightRangeHelp;
    bb::cascades::DropDown *_defaultOpenDirSelect;
    bb::cascades::Option *_chooseDefaultOpenDirOption;
    bb::cascades::Option *_currentDefaultOpenDirOption;
    bb::cascades::Label *_defaultOpenDirHelp;
    bb::cascades::pickers::FilePicker *_fpicker;
    Q_SLOT void resetDefaultOpenDirSelection();
    Q_SLOT void onChooseDefaultOpenDirSelectedChanged(bool);
    Q_SLOT void onDefaultOpenDirSelected(const QStringList &);
    Q_SLOT void onDefaultOpenDirectoryChanged(const QString &);
};

#endif /* GENERALSETTINGSPAGE_H_ */
