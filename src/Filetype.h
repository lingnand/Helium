/*
 * Filetype.h
 *
 *  Created on: May 15, 2015
 *      Author: lingnan
 */

#ifndef FILETYPE_H_
#define FILETYPE_H_

class RunProfileManager;

class Filetype : public QObject
{
    Q_OBJECT
public:
    Filetype(const QString &name=QString(),
            bool highlightEnabled=true,
            bool tabSpaceConversionEnabled=false,
            int numberOfSpacesForTab=4,
            RunProfileManager *runProfileManager=NULL,
            QObject *parent=NULL);
    const QString &name() const { return _name; }
    RunProfileManager *runProfileManager() const { return _runProfileManager; }
    void setRunProfileManager(RunProfileManager *);
    bool highlightEnabled() const { return _highlightEnabled; }
    // converts space to tab on load; and tab to space on save
    bool tabSpaceConversionEnabled() const { return _tabSpaceConversionEnabled; }
    int numberOfSpacesForTab() const { return _numberOfSpacesForTab; }
    Q_SLOT void setHighlightEnabled(bool);
    Q_SLOT void setTabSpaceConversionEnabled(bool);
    Q_SLOT void setNumberOfSpacesForTab(int);
Q_SIGNALS:
    void runProfileManagerChanged(RunProfileManager *change, RunProfileManager *old);
    void highlightEnabledChanged(bool);
    void tabSpaceConversionEnabledChanged(bool);
    void numberOfSpacesForTabChanged(int);
private:
    QString _name;
    bool _highlightEnabled;
    bool _tabSpaceConversionEnabled;
    int _numberOfSpacesForTab;
    RunProfileManager *_runProfileManager;
};
Q_DECLARE_METATYPE(Filetype *)

QDebug operator<<(QDebug, const Filetype *);

#endif /* FILETYPE_H_ */
