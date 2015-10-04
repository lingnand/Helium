/*
 * Versioner.h
 *
 *  Created on: Jul 30, 2015
 *      Author: lingnan
 */

#ifndef VERSIONER_H_
#define VERSIONER_H_

#include <QSettings>

// a simple class for versioning
class Version {
public:
    Version(int _v0=0, int _v1=0, int _v2=0, int _v3=0):
        v0(_v0), v1(_v1), v2(_v2), v3(_v3) {}
    Version(const QString &str) {
        QStringList list = str.split('.');
        v0 = list.size() > 0 ? list[0].toInt() : 0;
        v1 = list.size() > 1 ? list[1].toInt() : 0;
        v2 = list.size() > 2 ? list[2].toInt() : 0;
        v3 = list.size() > 3 ? list[3].toInt() : 0;
    }
    bool isNull() const { return v0 == 0 && v1 == 0 && v2 == 0 && v3 ==0; }
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
    bool operator>=(const Version &v) const {
        return operator==(v) || operator>(v);
    }
    bool operator<=(const Version &v) const {
        return operator==(v) || operator<(v);
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

// class for retrieve the current and past versions
class Versioner
{
public:
    Versioner();
    // the version number of the last launched instance
    const Version &last() const { return _last; }
    const Version &current() const { return _current; }
private:
    Version _last;
    Version _current;
    QSettings _settings;
};

#endif /* VERSIONER_H_ */
