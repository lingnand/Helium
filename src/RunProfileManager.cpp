/*
 * RunProfileManager.cpp
 *
 *  Created on: May 15, 2015
 *      Author: lingnan
 */

#include <RunProfileManager.h>
#include <CmdRunProfileManager.h>

Filetype *RunProfileManager::parent() const
{
    return (Filetype *) QObject::parent();
}
