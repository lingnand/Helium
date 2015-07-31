/*
 * GeneralSettings.h
 *
 *  Created on: May 25, 2015
 *      Author: lingnan
 */

#ifndef GENERALSETTINGS_H_
#define GENERALSETTINGS_H_

#include <QStringList>

class GeneralSettings : public QObject
{
    Q_OBJECT
public:
    GeneralSettings(int highlightRange, const QString &defaultProjectDirectory,
            int numberOfTimesLaunched, bool hasConfirmedSupport,
            QObject *parent=NULL):
        QObject(parent), _highlightRange(highlightRange), _defaultProjectDir(defaultProjectDirectory),
        _numberOfTimesLaunched(numberOfTimesLaunched),
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
    bool _hasConfirmedSupport;
};

#endif /* GENERALSETTINGS_H_ */
