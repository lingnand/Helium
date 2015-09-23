/*
 * GitSettingsPage.cpp
 *
 *  Created on: Sep 22, 2015
 *      Author: lingnan
 */

#include <bb/cascades/TitleBar>
#include <bb/cascades/Picker>
#include <bb/cascades/Label>
#include <bb/cascades/DropDown>
#include <bb/cascades/Option>
#include <bb/cascades/Divider>
#include <bb/cascades/TextField>
#include <bb/cascades/ScrollView>
#include <bb/cascades/Header>
#include <bb/cascades/pickers/FilePicker>
#include <GitSettingsPage.h>
#include <GitSettings.h>
#include <Utility.h>
#include <Segment.h>

using namespace bb::cascades;

GitSettingsPage::GitSettingsPage(GitSettings *settings):
    _settings(settings),
    _authorHeader(Header::create()),
    _nameField(TextField::create().text(settings->name())),
    _emailField(TextField::create()
        .inputMode(TextFieldInputMode::EmailAddress)
        .text(settings->email())),
    _authorHelp(Label::create().multiline(true)
        .textStyle(Defaults::helpText())),
    _sshHeader(Header::create()),
    _chooseSshPrivateKeyPathOption(Option::create()),
    _currentSshPrivateKeyPathOption(Option::create().text(settings->sshPrivateKeyPath())),
    _sshPrivateKeyPathSelect(DropDown::create()
        .add(_chooseSshPrivateKeyPathOption)
        .add(_currentSshPrivateKeyPathOption)),
    _chooseSshPublicKeyPathOption(Option::create()),
    _currentSshPublicKeyPathOption(Option::create().text(settings->sshPublicKeyPath())),
    _sshPublicKeyPathSelect(DropDown::create()
        .add(_chooseSshPublicKeyPathOption)
        .add(_currentSshPublicKeyPathOption)),
    _sshKeyPathHelp(Label::create().multiline(true)
        .textStyle(Defaults::helpText())),
    _sshUsernameField(TextField::create()
        .inputFlags(Defaults::codeInputFlags())
        .contentFlags(Defaults::codeContentFlags())
        .text(settings->sshUsername())),
    _sshUsernameHelp(Label::create().multiline(true)
        .textStyle(Defaults::helpText())),
    _sshPassphraseField(TextField::create()
        .inputMode(TextFieldInputMode::Password)
        .text(settings->sshPassphrase())),
    _sshPassphraseHelp(Label::create().multiline(true)
        .textStyle(Defaults::helpText())),
    _fpicker(NULL)
{
    setTitleBar(TitleBar::create());

    _sshPassphraseField->input()->setSubmitKey(SubmitKey::Done);
    _sshPassphraseField->input()->setSubmitKeyFocusBehavior(SubmitKeyFocusBehavior::Lose);
    conn(_nameField, SIGNAL(textChanged(const QString&)),
        settings, SLOT(setName(const QString&)));
    conn(settings, SIGNAL(nameChanged(const QString&)),
        _nameField, SLOT(setText(const QString&)));
    conn(_emailField, SIGNAL(textChanged(const QString&)),
        settings, SLOT(setEmail(const QString&)));
    conn(settings, SIGNAL(emailChanged(const QString&)),
        _emailField, SLOT(setText(const QString&)));

    resetKeyPathSelections();
    conn(_chooseSshPrivateKeyPathOption, SIGNAL(selectedChanged(bool)),
        this, SLOT(onChooseSshPrivateKeyPathSelectedChanged(bool)));
    conn(_chooseSshPublicKeyPathOption, SIGNAL(selectedChanged(bool)),
        this, SLOT(onChooseSshPublicKeyPathSelectedChanged(bool)));
    conn(settings, SIGNAL(sshPrivateKeyPathChanged(const QString&)),
        _currentSshPrivateKeyPathOption, SLOT(setText(const QString&)));
    conn(settings, SIGNAL(sshPublicKeyPathChanged(const QString&)),
        _currentSshPublicKeyPathOption, SLOT(setText(const QString&)))
    conn(_sshUsernameField, SIGNAL(textChanged(const QString&)),
        settings, SLOT(setSshUsername(const QString&)));
    conn(settings, SIGNAL(sshUsernameChanged(const QString&)),
        _sshUsernameField, SLOT(setText(const QString&)));
    conn(_sshPassphraseField, SIGNAL(textChanged(const QString&)),
        settings, SLOT(setSshPassphrase(const QString&)));
    conn(settings, SIGNAL(sshPassphraseChanged(const QString&)),
        _sshPassphraseField, SLOT(setText(const QString&)));

    setContent(ScrollView::create(Segment::create().section()
            .add(_authorHeader)
            .add(Segment::create().subsection().add(_nameField))
            .add(Segment::create().subsection().add(_emailField))
            .add(Segment::create().subsection().add(_authorHelp))
            .add(_sshHeader)
            .add(Segment::create().subsection().add(_sshPrivateKeyPathSelect))
            .add(Segment::create().subsection().add(_sshPublicKeyPathSelect))
            .add(Segment::create().subsection().add(_sshKeyPathHelp))
            .add(Divider::create())
            .add(Segment::create().subsection().add(_sshUsernameField))
            .add(Segment::create().subsection().add(_sshUsernameHelp))
            .add(Segment::create().subsection().add(_sshPassphraseField))
            .add(Segment::create().subsection().add(_sshPassphraseHelp)))
        .scrollMode(ScrollMode::Vertical));

    onTranslatorChanged();
}

void GitSettingsPage::onTranslatorChanged()
{
    PushablePage::onTranslatorChanged();
    titleBar()->setTitle(tr("Git Settings"));
    _authorHeader->setTitle(tr("Author"));
    _nameField->setHintText(tr("Name"));
    _emailField->setHintText(tr("Email"));
    _authorHelp->setText(tr("Name and email used in commit and reflog"));
    _sshHeader->setTitle(tr("SSH Authentication"));
    _sshPrivateKeyPathSelect->setTitle(tr("SSH Private Key"));
    _chooseSshPrivateKeyPathOption->setText(tr("<Choose a Private Key File>"));
    _sshPublicKeyPathSelect->setTitle(tr("SSH Public Key"));
    _chooseSshPublicKeyPathOption->setText(tr("<Choose a Public Key File>"));
    _sshKeyPathHelp->setText(tr("Path to private/public key file for SSH authentication"));
    _sshUsernameField->setHintText(tr("Username"));
    _sshUsernameHelp->setText(tr("SSH username, used when the remote URL doesn't have one"));
    _sshPassphraseField->setHintText(tr("Passphrase"));
    _sshPassphraseHelp->setText(tr("SSH passphrase"));
}

void GitSettingsPage::resetKeyPathSelections()
{
    _sshPrivateKeyPathSelect->setSelectedOption(_currentSshPrivateKeyPathOption);
    _sshPublicKeyPathSelect->setSelectedOption(_currentSshPublicKeyPathOption);
}

pickers::FilePicker *GitSettingsPage::fpicker(const char *fileSelectedHandler)
{
    if (!_fpicker) {
        _fpicker = new pickers::FilePicker(this);
        _fpicker->setType(pickers::FileType::Document);
        _fpicker->setFilter(QStringList("*"));
        _fpicker->setMode(pickers::FilePickerMode::Picker);
        conn(_fpicker, SIGNAL(pickerClosed()),
            this, SLOT(resetKeyPathSelections()));
    }
    _fpicker->disconnect(SIGNAL(fileSelected(const QStringList&)));
    conn(_fpicker, SIGNAL(fileSelected(const QStringList&)),
        this, fileSelectedHandler);
    return _fpicker;
}

void GitSettingsPage::onChooseSshPrivateKeyPathSelectedChanged(bool selected)
{
    if (selected)
        fpicker(SLOT(onSshPrivateKeyPathSelected(const QStringList&)))->open();
}

void GitSettingsPage::onSshPrivateKeyPathSelected(const QStringList &list)
{
    _settings->setSshPrivateKeyPath(list[0]);
}

void GitSettingsPage::onChooseSshPublicKeyPathSelectedChanged(bool selected)
{
    if (selected)
        fpicker(SLOT(onSshPublicKeyPathSelected(const QStringList&)))->open();
}

void GitSettingsPage::onSshPublicKeyPathSelected(const QStringList &list)
{
    _settings->setSshPublicKeyPath(list[0]);
}