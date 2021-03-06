/*
 * WebRunProfileSettingsUI.h
 *
 *  Created on: Jun 4, 2015
 *      Author: lingnan
 */

#ifndef WEBRUNPROFILESETTINGSUI_H_
#define WEBRUNPROFILESETTINGSUI_H_

#include <RunProfileSettingsUI.h>
#include <WebRunProfile.h>

namespace bb {
    namespace cascades {
        class Header;
        class Label;
        class DropDown;
        class Option;
    }
}

class WebRunProfileManager;

class WebRunProfileSettingsUI : public RunProfileSettingsUI
{
    Q_OBJECT
public:
    WebRunProfileSettingsUI(WebRunProfileManager *);
    virtual ~WebRunProfileSettingsUI() {}
    void onTranslatorChanged();
private:
    WebRunProfileManager *_manager;
    bb::cascades::Header *_header;
    bb::cascades::Option *_htmlOption;
    bb::cascades::Option *_javascriptOption;
    bb::cascades::Option *_markdownOption;
    bb::cascades::DropDown *_modeSelect;
    bb::cascades::Label *_help;
    Q_SLOT void onModeSelectionChanged(const QVariant &);
    Q_SLOT void onManagerModeChanged(WebRunProfile::Mode);
};

#endif /* WEBRUNPROFILESETTINGSUI_H_ */
