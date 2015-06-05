/*
 * WebRunProfileManager.cpp
 *
 *  Created on: Jun 4, 2015
 *      Author: lingnan
 */

#include <WebRunProfileManager.h>
#include <WebRunProfile.h>
#include <Utility.h>

WebRunProfileManager::WebRunProfileManager(WebRunProfile::Mode mode): _mode(mode)
{}

RunProfile *WebRunProfileManager::createRunProfile(View *view)
{
    WebRunProfile *p = new WebRunProfile(view, _mode);
    conn(this, SIGNAL(modeChanged(WebRunProfile::Mode)),
            p, SLOT(setMode(WebRunProfile::Mode)));
    return p;
}

void WebRunProfileManager::setMode(WebRunProfile::Mode mode)
{
    if (mode != _mode) {
        _mode = mode;
        emit modeChanged(_mode);
    }
}
