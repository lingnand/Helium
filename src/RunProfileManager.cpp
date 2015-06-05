/*
 * RunProfileManager.cpp
 *
 *  Created on: May 15, 2015
 *      Author: lingnan
 */

#include <RunProfileManager.h>
#include <CmdRunProfileManager.h>
#include <WebRunProfileManager.h>

RunProfileManager *RunProfileManager::create(RunProfileManager::Type type)
{
    switch (type) {
        case RunProfileManager::None:
            return NULL;
        case RunProfileManager::Cmd:
            return new CmdRunProfileManager;
        case RunProfileManager::Web:
            return new WebRunProfileManager;
    }
}

RunProfileManager::Type RunProfileManager::type(RunProfileManager *manager)
{
    if (dynamic_cast<CmdRunProfileManager *>(manager))
        return RunProfileManager::Cmd;
    if (dynamic_cast<WebRunProfileManager *>(manager))
        return RunProfileManager::Web;
    return RunProfileManager::None;
}

Filetype *RunProfileManager::parent() const
{
    return (Filetype *) QObject::parent();
}
