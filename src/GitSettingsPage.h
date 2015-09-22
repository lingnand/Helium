/*
 * GitSettingsPage.h
 *
 *  Created on: Sep 22, 2015
 *      Author: lingnan
 */

#ifndef GITSETTINGSPAGE_H_
#define GITSETTINGSPAGE_H_

#include <PushablePage.h>

namespace bb {
    namespace cascades {
        class Label;
        class DropDown;
        class Option;
        class TextField;
        namespace pickers {
            class FilePicker;
        }
    }
}

class GitSettings;

class GitSettingsPage : public PushablePage
{
    Q_OBJECT
public:
    GitSettingsPage(GitSettings *);
    void onTranslatorChanged();
private:
    GitSettings *_settings;
    bb::cascades::TextField *_usernameField;
    bb::cascades::Label *_usernameHelp;
    bb::cascades::TextField *_emailField;
    bb::cascades::Label *_emailHelp;
    bb::cascades::DropDown *_sshPrivateKeyPathSelect;
    bb::cascades::Option *_chooseSshPrivateKeyPathOption;
    bb::cascades::Option *_currentSshPrivateKeyPathOption;
    bb::cascades::Label *_sshPrivateKeyPathHelp;
    bb::cascades::DropDown *_sshPublicKeyPathSelect;
    bb::cascades::Option *_chooseSshPublicKeyPathOption;
    bb::cascades::Option *_currentSshPublicKeyPathOption;
    bb::cascades::Label *_sshPublicKeyPathHelp;
    bb::cascades::TextField *_sshUsernameField;
    bb::cascades::Label *_sshUsernameHelp;
    bb::cascades::TextField *_sshPassphraseField;
    bb::cascades::Label *_sshPassphraseHelp;
    bb::cascades::pickers::FilePicker *_fpicker;
};

#endif /* GITSETTINGSPAGE_H_ */