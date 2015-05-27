/*
 * Filetype.h
 *
 *  Created on: May 15, 2015
 *      Author: lingnan
 */

#ifndef FILETYPE_H_
#define FILETYPE_H_

#include <HighlightType.h>

class RunProfileManager;

class Filetype : public QObject
{
    Q_OBJECT
public:
    Filetype(const QString &name=QString(),
            bool highlightEnabled=true,
            RunProfileManager *runProfileManager=NULL,
            QObject *parent=NULL);
    const QString &name() const { return _name; }
    std::string langName() const; // for working with srchilite
    HighlightType highlightType() { return HighlightType(this, _highlightEnabled); }
    RunProfileManager *runProfileManager() const { return _runProfileManager; }
    void setRunProfileManager(RunProfileManager *);
    bool highlightEnabled() const { return _highlightEnabled; }
    Q_SLOT void setHighlightEnabled(bool);
Q_SIGNALS:
    void runProfileManagerChanged(RunProfileManager *change, RunProfileManager *old);
    void highlightEnabledChanged(bool);
    void highlightTypeChanged(const HighlightType &);
private:
    QString _name;
    bool _highlightEnabled;
    RunProfileManager *_runProfileManager;
};
Q_DECLARE_METATYPE(Filetype *)

QDebug operator<<(QDebug, const Filetype *);

#endif /* FILETYPE_H_ */
