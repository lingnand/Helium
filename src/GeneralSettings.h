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
    GeneralSettings(int highlightRange, const QString &defaultOpenDirectory, QObject *parent=NULL):
        QObject(parent), _highlightRange(highlightRange), _defaultOpenDir(defaultOpenDirectory) {}
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
Q_SIGNALS:
    void highlightRangeChanged(int);
    void defaultOpenDirectoryChanged(const QString &);
private:
    int _highlightRange;
    QString _defaultOpenDir;
};

#endif /* GENERALSETTINGS_H_ */
