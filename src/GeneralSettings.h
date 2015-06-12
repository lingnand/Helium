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
    GeneralSettings(int highlightRange, QObject *parent=NULL):
        QObject(parent), _highlightRange(highlightRange) {}
    int highlightRange() const { return _highlightRange; }
    Q_SLOT void setHighlightRange(int r) {
        if (r != _highlightRange) {
            _highlightRange = r;
            emit highlightRangeChanged(_highlightRange);
        }
    }
Q_SIGNALS:
    void highlightRangeChanged(int);
private:
    int _highlightRange;
};

#endif /* GENERALSETTINGS_H_ */
