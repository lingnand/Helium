/*
 * RunProfileSettingsUI.cpp
 *
 *  Created on: May 24, 2015
 *      Author: lingnan
 */

#include <RunProfileSettingsUI.h>
#include <CmdRunProfileManager.h>
#include <CmdRunProfileSettingsUI.h>
#include <WebRunProfileManager.h>
#include <WebRunProfileSettingsUI.h>

using namespace bb::cascades;

RunProfileSettingsUI *RunProfileSettingsUI::create(RunProfileManager *manager)
{
    if (CmdRunProfileManager *cp = dynamic_cast<CmdRunProfileManager *>(manager)) {
        return new CmdRunProfileSettingsUI(cp);
    }
    if (WebRunProfileManager *wp = dynamic_cast<WebRunProfileManager *>(manager)) {
        return new WebRunProfileSettingsUI(wp);
    }
    return NULL;
}
