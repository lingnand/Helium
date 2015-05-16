/*
 * CmdRunProileManager.h
 *
 *  Created on: May 15, 2015
 *      Author: lingnan
 */

#ifndef CMDRUNPROFILEMANAGER_H_
#define CMDRUNPROFILEMANAGER_H_

#include <RunProfileManager.h>

class CmdRunProfileManager: public RunProfileManager
{
    Q_OBJECT
public:
    CmdRunProfileManager(const QString &cmd, QObject *parent=NULL);
    RunProfile *createRunProfile(View *);
    void setCmd(const QString &cmd);
Q_SIGNALS:
    void cmdChanged(const QString &cmd);
private:
    QString _cmd;
};

#endif /* CMDRUNPROFILEMANAGER_H_ */
