/*
 * GeneralSettingsPage.h
 *
 *  Created on: May 25, 2015
 *      Author: lingnan
 */

#ifndef GENERALSETTINGSPAGE_H_
#define GENERALSETTINGSPAGE_H_

#include <PushablePage.h>

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

class GeneralSettingsPage : public PushablePage
{
    Q_OBJECT
public:
    GeneralSettingsPage(GeneralSettings *);
    Q_SLOT void onTranslatorChanged();
private:
    GeneralSettings *_settings;
    NumberPicker *_highlightRangePicker;
    bb::cascades::Label *_highlightRangeHelp;
    bb::cascades::Option *_chooseDefaultProjectDirOption;
    bb::cascades::Option *_currentDefaultProjectDirOption;
    bb::cascades::DropDown *_defaultProjectDirSelect;
    bb::cascades::Label *_defaultProjectDirHelp;
    bb::cascades::pickers::FilePicker *_fpicker;
    Q_SLOT void resetDefaultProjectDirSelection();
    Q_SLOT void onChooseDefaultProjectDirSelectedChanged(bool);
    Q_SLOT void onDefaultProjectDirSelected(const QStringList &);
    Q_SLOT void onDefaultProjectDirectoryChanged(const QString &);
};

#endif /* GENERALSETTINGSPAGE_H_ */
