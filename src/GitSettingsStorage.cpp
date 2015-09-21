/*
 * GitSettingsStorage.cpp
 *
 *  Created on: Sep 21, 2015
 *      Author: lingnan
 */

#include <GitSettingsStorage.h>
#include <GitSettings.h>
#include <Utility.h>

GitSettingsStorage::GitSettingsStorage(const QString &prefix, QObject *parent):
    QObject(parent)
{
    _settings.beginGroup(prefix);
}

void GitSettingsStorage::onNameChanged(const QString &name)
{
    _settings.setValue("name", name);
}

void GitSettingsStorage::onEmailChanged(const QString &email)
{
    _settings.setValue("email", email);
}

void GitSettingsStorage::onSshPrivateKeyPathChanged(const QString &path)
{
    _settings.setValue("ssh_private_key_path", path);
}

void GitSettingsStorage::onSshPublicKeyPathChanged(const QString &path)
{
    _settings.setValue("ssh_public_key_path", path);
}

void GitSettingsStorage::onSshUsernameChanged(const QString &sshUsername)
{
    _settings.setValue("ssh_username", sshUsername);
}

void GitSettingsStorage::onSshPassphraseChanged(const QString &sshPassphrase)
{
    _settings.setValue("ssh_passphrase", sshPassphrase);
}

GitSettings *GitSettingsStorage::read()
{
    qDebug() << "Reading git settings...";
    GitSettings *settings = new GitSettings(
        _settings.value("name").toString(),
        _settings.value("email").toString(),
        _settings.value("ssh_private_key_path").toString(),
        _settings.value("ssh_public_key_path").toString(),
        _settings.value("ssh_username").toString(),
        _settings.value("ssh_passphrase").toString(),
        this);
    conn(settings, SIGNAL(nameChanged(const QString&)),
        this, SLOT(onNameChanged(const QString&)));
    conn(settings, SIGNAL(emailChanged(const QString&)),
        this, SLOT(onEmailChanged(const QString&)));
    conn(settings, SIGNAL(sshPrivateKeyPathChanged(const QString&)),
        this, SLOT(onSshPrivateKeyPathChanged(const QString&)));
    conn(settings, SIGNAL(sshPublicKeyPathChanged(const QString&)),
        this, SLOT(onSshPublicKeyPathChanged(const QString&)));
    conn(settings, SIGNAL(sshUsernameChanged(const QString&)),
        this, SLOT(onSshUsernameChanged(const QString&)));
    conn(settings, SIGNAL(sshPassphraseChanged(const QString&)),
        this, SLOT(onSshPassphraseChanged(const QString&)));
    return settings;
}
