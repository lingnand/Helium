/*
 * GitSettings.h
 *
 *  Created on: Aug 16, 2015
 *      Author: lingnan
 */

#ifndef GITSETTINGS_H_
#define GITSETTINGS_H_

#include <libqgit2/qgitsignature.h>

class GitSettings : public QObject
{
    Q_OBJECT
public:
    GitSettings(const QString &name, const QString &email):
        _name(name), _email(email) {}
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
Q_SIGNALS:
    void nameChanged(const QString &);
    void emailChanged(const QString &);
private:
    QString _name, _email;
};

#endif /* GITSETTINGS_H_ */
