/*
 * CmdRunProfileSettings.h
 *
 *  Created on: May 24, 2015
 *      Author: lingnan
 */

#ifndef CMDRUNPROFILESETTINGS_H_
#define CMDRUNPROFILESETTINGS_H_

#include <RunProfileSettings.h>

namespace bb {
    namespace cascades {
        class Header;
        class Label;
        class TextField;
    }
}

class CmdRunProfileManager;

class CmdRunProfileSettings : public RunProfileSettings
{
    Q_OBJECT
public:
    CmdRunProfileSettings(CmdRunProfileManager *);
    virtual ~CmdRunProfileSettings() {}
    void onTranslatorChanged();
private:
    bb::cascades::Header *_header;
    bb::cascades::TextField *_field;
    bb::cascades::Label *_help;
    void reloadHeader();
    Q_SLOT void onCmdChanged(const QString&);
};

#endif /* CMDRUNPROFILESETTINGS_H_ */
