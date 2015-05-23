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
            RunProfileManager *runProfileManager=NULL,
            QObject *parent=NULL);
    const QString &name() const;
    std::string langName() const; // for working with srchilite
    RunProfileManager *runProfileManager() const;
    void setRunProfileManager(RunProfileManager *);
    bool highlightEnabled() const;
    Q_SLOT void setHighlightEnabled(bool);
Q_SIGNALS:
    void runProfileManagerChanged(RunProfileManager *change, RunProfileManager *old);
    void highlightEnabledChanged(bool);
private:
    QString _name;
    bool _highlightEnabled;
    RunProfileManager *_runProfileManager;
};
Q_DECLARE_METATYPE(Filetype *)

QDebug operator<<(QDebug, const Filetype *);

#endif /* FILETYPE_H_ */
