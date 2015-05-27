/*
 * RunProfileSettingsUI.cpp
 *
 *  Created on: May 24, 2015
 *      Author: lingnan
 */

#include <RunProfileSettingsUI.h>
#include <CmdRunProfileManager.h>
#include <CmdRunProfileSettingsUI.h>

using namespace bb::cascades;

RunProfileSettingsUI *RunProfileSettingsUI::create(RunProfileManager *manager)
{
    if (CmdRunProfileManager *cp = dynamic_cast<CmdRunProfileManager *>(manager)) {
        return new CmdRunProfileSettingsUI(cp);
    }
    return NULL;
}
