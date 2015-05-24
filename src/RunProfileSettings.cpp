/*
 * RunProfileSettings.cpp
 *
 *  Created on: May 24, 2015
 *      Author: lingnan
 */

#include <RunProfileSettings.h>
#include <CmdRunProfileManager.h>
#include <CmdRunProfileSettings.h>

using namespace bb::cascades;

RunProfileSettings *RunProfileSettings::create(RunProfileManager *manager)
{
    if (CmdRunProfileManager *cp = dynamic_cast<CmdRunProfileManager *>(manager)) {
        return new CmdRunProfileSettings(cp);
    }
    return NULL;
}
