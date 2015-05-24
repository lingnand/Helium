/*
 * CmdRunProfileManager.cpp
 *
 *  Created on: May 15, 2015
 *      Author: lingnan
 */

#include <QDebug>
#include <CmdRunProfileManager.h>
#include <CmdRunProfile.h>
#include <Utility.h>

CmdRunProfileManager::CmdRunProfileManager(const QString &cmd): _cmd(cmd)
{}

RunProfile *CmdRunProfileManager::createRunProfile(View *view)
{
    CmdRunProfile *p = new CmdRunProfile(view, _cmd);
    conn(this, SIGNAL(cmdChanged(const QString&)), p, SLOT(setCmd(const QString&)));
    return p;
}

const QString &CmdRunProfileManager::cmd() const
{
    return _cmd;
}

void CmdRunProfileManager::setCmd(const QString &cmd)
{
    if (cmd != _cmd) {
        _cmd = cmd;
        emit cmdChanged(_cmd);
    }
}
