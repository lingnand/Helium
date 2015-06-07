/*
 * CmdRunProfileSettingsUI.h
 *
 *  Created on: May 24, 2015
 *      Author: lingnan
 */

#ifndef CMDRUNPROFILESETTINGSUI_H_
#define CMDRUNPROFILESETTINGSUI_H_

#include <RunProfileSettingsUI.h>

namespace bb {
    namespace cascades {
        class Header;
        class Label;
        class TextField;
    }
}

class CmdRunProfileManager;

class CmdRunProfileSettingsUI : public RunProfileSettingsUI
{
    Q_OBJECT
public:
    CmdRunProfileSettingsUI(CmdRunProfileManager *);
    virtual ~CmdRunProfileSettingsUI() {}
    void onTranslatorChanged();
private:
    bb::cascades::Header *_header;
    bb::cascades::TextField *_field;
    bb::cascades::Label *_help;
};

#endif /* CMDRUNPROFILESETTINGSUI_H_ */
