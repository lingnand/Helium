/*
 * FilePropertiesPage.h
 *
 *  Created on: May 23, 2015
 *      Author: lingnan
 */

#ifndef FILEPROPERTIESPAGE_H_
#define FILEPROPERTIESPAGE_H_

#include <bb/cascades/Page>

namespace bb {
    namespace cascades {
        class ToggleButton;
        class Label;
        class DropDown;
        class Option;
        class ActionItem;
    }
}

class Filetype;
class FiletypeSettingsUI;
class Segment;

class FilePropertiesPage : public bb::cascades::Page
{
    Q_OBJECT
public:
    FilePropertiesPage();
    Q_SLOT void setFiletype(Filetype *);
    Q_SLOT void setAutodetectFiletypeChecked(bool checked);
    Q_SLOT void onTranslatorChanged();
Q_SIGNALS:
    void autodetectFiletypeCheckedChanged(bool);
    void filetypeSelectionChanged(Filetype *);
    void backButtonTriggered();
private:
    bb::cascades::Label *_autodetectFiletypeToggleLabel;
    bb::cascades::ToggleButton *_autodetectFiletypeToggle;
    bb::cascades::Label *_autodetectFiletypeToggleHelp;
    bb::cascades::Option *_noneFiletypeOption;
    bb::cascades::DropDown *_filetypeSelect;
    bb::cascades::Label *_filetypeSelectHelp;
    FiletypeSettingsUI *_filetypeSettingsUI;
    bb::cascades::ActionItem *_backButton;
    Q_SLOT void onFiletypeSelectionChanged(const QVariant &);
};

#endif /* FILEPROPERTIESPAGE_H_ */
