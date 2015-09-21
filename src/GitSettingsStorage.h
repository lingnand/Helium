/*
 * GitSettingsStorage.h
 *
 *  Created on: Sep 21, 2015
 *      Author: lingnan
 */

#ifndef GITSETTINGSSTORAGE_H_
#define GITSETTINGSSTORAGE_H_

#include <QSettings>

class GitSettings;

class GitSettingsStorage : public QObject
{
    Q_OBJECT
public:
    GitSettingsStorage(const QString &prefix, QObject *parent=NULL);
    GitSettings *read();
private:
    QSettings _settings;
    Q_SLOT void onNameChanged(const QString &);
    Q_SLOT void onEmailChanged(const QString &);
    Q_SLOT void onSshPrivateKeyPathChanged(const QString &);
    Q_SLOT void onSshPublicKeyPathChanged(const QString &);
    Q_SLOT void onSshUsernameChanged(const QString &);
    Q_SLOT void onSshPassphraseChanged(const QString &);
};

#endif /* GITSETTINGSSTORAGE_H_ */
