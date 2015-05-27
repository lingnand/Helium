/*
 * FiletypeSettingsUI.h
 *
 *  Created on: May 21, 2015
 *      Author: lingnan
 */

#ifndef FILETYPESETTINGSUI_H_
#define FILETYPESETTINGSUI_H_

#include <bb/cascades/Container>

namespace bb {
    namespace cascades {
        class Header;
        class ToggleButton;
        class Label;
        class DropDown;
        class Option;
    }
}

class Filetype;
class RunProfileManager;
class RunProfileSettingsUI;

class FiletypeSettingsUI : public bb::cascades::Container
{
    Q_OBJECT
public:
    FiletypeSettingsUI(Filetype *filetype=NULL);
    void setFiletype(Filetype *);
    Q_SLOT void onTranslatorChanged();
private:
    Filetype *_filetype;
    bb::cascades::Header *_header;
    bb::cascades::ToggleButton *_highlightToggle;
    bb::cascades::Label *_highlightToggleLabel;
    bb::cascades::Label *_highlightToggleHelp;
    bb::cascades::Header *_runProfileHeader;
    bb::cascades::DropDown *_runProfileSelect;
    bb::cascades::Option *_noneRunProfileOption;
    bb::cascades::Option *_cmdRunProfileOption;
    RunProfileSettingsUI *_runProfileSettingsUI;

    void reloadHeader();
    Q_SLOT void onHighlightCheckedChanged(bool);
    Q_SLOT void onFiletypeRunProfileManagerChanged(RunProfileManager *change);
    Q_SLOT void onRunProfileSelectionChanged(const QVariant);
};

#endif /* FILETYPESETTINGSUI_H_ */
