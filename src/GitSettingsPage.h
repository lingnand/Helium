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
        class Header;
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
    Q_SLOT void onTranslatorChanged();
private:
    GitSettings *_settings;
    bb::cascades::Header *_authorHeader;
    bb::cascades::TextField *_nameField;
    bb::cascades::TextField *_emailField;
    bb::cascades::Label *_authorHelp;
    bb::cascades::Header *_sshHeader;
    bb::cascades::Option *_chooseSshPrivateKeyPathOption;
    bb::cascades::Option *_currentSshPrivateKeyPathOption;
    bb::cascades::DropDown *_sshPrivateKeyPathSelect;
    bb::cascades::Option *_chooseSshPublicKeyPathOption;
    bb::cascades::Option *_currentSshPublicKeyPathOption;
    bb::cascades::DropDown *_sshPublicKeyPathSelect;
    bb::cascades::Label *_sshKeyPathHelp;
    bb::cascades::TextField *_sshUsernameField;
    bb::cascades::Label *_sshUsernameHelp;
    bb::cascades::TextField *_sshPassphraseField;
    bb::cascades::Label *_sshPassphraseHelp;
    bb::cascades::pickers::FilePicker *_fpicker;

    bb::cascades::pickers::FilePicker *fpicker(const char *fileSelectHandler);
    Q_SLOT void resetKeyPathSelections();
    Q_SLOT void onChooseSshPrivateKeyPathSelectedChanged(bool);
    Q_SLOT void onSshPrivateKeyPathSelected(const QStringList &);
    Q_SLOT void onChooseSshPublicKeyPathSelectedChanged(bool);
    Q_SLOT void onSshPublicKeyPathSelected(const QStringList &);
};

#endif /* GITSETTINGSPAGE_H_ */
