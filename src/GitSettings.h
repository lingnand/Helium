/*
 * GitSettings.h
 *
 *  Created on: Aug 16, 2015
 *      Author: lingnan
 */

#ifndef GITSETTINGS_H_
#define GITSETTINGS_H_

#include <libqgit2/qgitsignature.h>
#include <libqgit2/qgitcredentials.h>

class GitSettings : public QObject
{
    Q_OBJECT
public:
    GitSettings(const QString &name, const QString &email,
            const QString &sshPrivateKeyPath, const QString &sshPublicKeyPath,
            const QString &sshUsername, const QString &sshPassphrase,
            QObject *parent=NULL):
        QObject(parent),
        _name(name), _email(email),
        _sshPrivateKeyPath(sshPrivateKeyPath), _sshPublicKeyPath(sshPublicKeyPath),
        _sshUsername(sshUsername), _sshPassphrase(sshPassphrase) {}
    const QString &name() const { return _name; }
    const QString &email() const { return _email; }
    Q_SLOT void setName(const QString &n) {
        if (n != _name) {
            _name = n;
            emit nameChanged(_name);
        }
    }
    Q_SLOT void setEmail(const QString &em) {
        if (em != _email) {
            _email = em;
            emit emailChanged(_email);
        }
    }
    LibQGit2::Signature signature() const {
        return LibQGit2::Signature(_name, _email);
    }
    const QString &sshPrivateKeyPath() const { return _sshPrivateKeyPath; }
    const QString &sshPublicKeyPath() const { return _sshPublicKeyPath; }
    const QString &sshUsername() const { return _sshUsername; }
    const QString &sshPassphrase() const { return _sshPassphrase; }
    LibQGit2::Credentials sshCredentials() const {
        return LibQGit2::Credentials::ssh(_sshPrivateKeyPath, _sshPublicKeyPath,
                _sshUsername.toLocal8Bit(), _sshPassphrase.toLocal8Bit());
    }
    Q_SLOT void setSshPrivateKeyPath(const QString &p) {
        if (p != _sshPrivateKeyPath) {
            _sshPrivateKeyPath = p;
            emit sshPrivateKeyPathChanged(_sshPrivateKeyPath);
        }
    }
    Q_SLOT void setSshPublicKeyPath(const QString &p) {
        if (p != _sshPublicKeyPath) {
            _sshPublicKeyPath = p;
            emit sshPublicKeyPathChanged(_sshPublicKeyPath);
        }
    }
    Q_SLOT void setSshUsername(const QString &u) {
        if (u != _sshUsername) {
            _sshUsername = u;
            emit sshUsernameChanged(_sshUsername);
        }
    }
    Q_SLOT void setSshPassphrase(const QString &p) {
        if (p != _sshPassphrase) {
            _sshPassphrase = p;
            emit sshPassphraseChanged(_sshPassphrase);
        }
    }
Q_SIGNALS:
    void nameChanged(const QString &);
    void emailChanged(const QString &);
    void sshPrivateKeyPathChanged(const QString &);
    void sshPublicKeyPathChanged(const QString &);
    void sshUsernameChanged(const QString &);
    void sshPassphraseChanged(const QString &);
private:
    QString _name, _email;
    QString _sshPrivateKeyPath, _sshPublicKeyPath;
    QString _sshUsername, _sshPassphrase;
};

#endif /* GITSETTINGS_H_ */
