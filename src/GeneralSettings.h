/*
 * GeneralSettings.h
 *
 *  Created on: May 25, 2015
 *      Author: lingnan
 */

#ifndef GENERALSETTINGS_H_
#define GENERALSETTINGS_H_

class GeneralSettings : public QObject
{
    Q_OBJECT
public:
    GeneralSettings(int highlightRange, const QString &defaultOpenDirectory,
            int numberOfTimesLaunched, bool hasConfirmedSupport,
            QObject *parent=NULL):
        QObject(parent), _highlightRange(highlightRange), _defaultOpenDir(defaultOpenDirectory),
        _numberOfTimesLaunched(numberOfTimesLaunched), _hasConfirmedSupport(hasConfirmedSupport) {}
    int highlightRange() const { return _highlightRange; }
    Q_SLOT void setHighlightRange(int r) {
        if (r != _highlightRange) {
            _highlightRange = r;
            emit highlightRangeChanged(_highlightRange);
        }
    }
    QString defaultOpenDirectory() const { return _defaultOpenDir; }
    Q_SLOT void setDefaultOpenDirectory(const QString &dir)  {
        if (dir != _defaultOpenDir) {
            _defaultOpenDir = dir;
            emit defaultOpenDirectoryChanged(_defaultOpenDir);
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
    void defaultOpenDirectoryChanged(const QString &);
    void confirmedSupport();
private:
    int _highlightRange;
    QString _defaultOpenDir;
    int _numberOfTimesLaunched;
    bool _hasConfirmedSupport;
};

#endif /* GENERALSETTINGS_H_ */
