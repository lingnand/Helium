/*
 * GeneralSettings.h
 *
 *  Created on: May 25, 2015
 *      Author: lingnan
 */

#ifndef GENERALSETTINGS_H_
#define GENERALSETTINGS_H_

#include <QStringList>
#include <QDebug>

// a simple class for versioning
class Version {
public:
    Version(const QString &str) {
        QStringList list = str.split('.');
        v0 = list.size() > 0 ? list[0].toInt() : 0;
        v1 = list.size() > 1 ? list[1].toInt() : 0;
        v2 = list.size() > 2 ? list[2].toInt() : 0;
        v3 = list.size() > 3 ? list[3].toInt() : 0;
    }
    bool operator<(const Version &v) const {
        QList<int> l, lv;
        l << v0 << v1 << v2 << v3;
        lv << v.v0 << v.v1 << v.v2 << v.v3;
        for (int i = 0; i < l.size(); i++) {
            if (l[i] < lv[i])
                return true;
            if (l[i] > lv[i])
                return false;
        }
        return false;
    }
    bool operator>(const Version &v) const {
        return !operator<(v);
    }
    bool operator==(const Version &v) const {
        return v0 == v.v0 && v1 == v.v1 && v2 == v.v2 && v3 == v.v3;
    }
    bool operator!=(const Version &v) const {
        return !operator==(v);
    }
    QString string() const {
        return QString("%1.%2.%3.%4").arg(v0).arg(v1).arg(v2).arg(v3);
    }
    int v0, v1, v2, v3;
};

class GeneralSettings : public QObject
{
    Q_OBJECT
public:
    GeneralSettings(int highlightRange, const QString &defaultProjectDirectory,
            int numberOfTimesLaunched,
            const Version &lastVersion, const Version &currentVersion,
            bool hasConfirmedSupport,
            QObject *parent=NULL):
        QObject(parent), _highlightRange(highlightRange), _defaultProjectDir(defaultProjectDirectory),
        _numberOfTimesLaunched(numberOfTimesLaunched),
        _lastVersion(lastVersion), _currentVersion(currentVersion),
        _hasConfirmedSupport(hasConfirmedSupport) {}
    int highlightRange() const { return _highlightRange; }
    Q_SLOT void setHighlightRange(int r) {
        if (r != _highlightRange) {
            _highlightRange = r;
            emit highlightRangeChanged(_highlightRange);
        }
    }
    QString defaultProjectDirectory() const { return _defaultProjectDir; }
    Q_SLOT void setDefaultProjectDirectory(const QString &dir)  {
        if (dir != _defaultProjectDir) {
            _defaultProjectDir = dir;
            emit defaultProjectDirectoryChanged(_defaultProjectDir);
        }
    }
    // the version number of the last launched instance
    Version lastVersion() const { return _lastVersion; }
    Version currentVerison() const { return _currentVersion; }
    int numberOfTimesLaunched() const { return _numberOfTimesLaunched; };
    bool hasConfirmedSupport() const { return _hasConfirmedSupport; };
    void confirmSupport() {
        if (!_hasConfirmedSupport) {
            _hasConfirmedSupport = true;
            emit confirmedSupport();
        }
    }
Q_SIGNALS:
    void highlightRangeChanged(int);
    void defaultProjectDirectoryChanged(const QString &);
    void confirmedSupport();
private:
    int _highlightRange;
    QString _defaultProjectDir;
    int _numberOfTimesLaunched;
    Version _lastVersion;
    Version _currentVersion;
    bool _hasConfirmedSupport;
};

#endif /* GENERALSETTINGS_H_ */
