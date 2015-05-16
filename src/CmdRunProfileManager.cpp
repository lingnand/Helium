/*
 * CmdRunProfileManager.cpp
 *
 *  Created on: May 15, 2015
 *      Author: lingnan
 */

#include <CmdRunProfileManager.h>
#include <CmdRunProfile.h>
#include <Utility.h>

CmdRunProfileManager::CmdRunProfileManager(const QString &cmd, QObject *parent):
    RunProfileManager(parent), _cmd(cmd)
{

}

RunProfile *CmdRunProfileManager::createRunProfile(View *view)
{
    CmdRunProfile *p = new CmdRunProfile(view, _cmd);
    conn(this, SIGNAL(cmdChanged(const QString&)), p, SLOT(setCmd(const QString&)));
    return p;
}

void CmdRunProfileManager::setCmd(const QString &cmd)
{
    if (cmd != _cmd) {
        _cmd = cmd;
        emit cmdChanged(_cmd);
    }
}
