/*
 * RunProfile.cpp
 *
 *  Created on: May 9, 2015
 *      Author: lingnan
 */

#include <RunProfile.h>
#include <CmdRunProfile.h>
#include <View.h>

RunProfile *RunProfile::createRunProfile(View *view, const QString &filetype)
{
    if (filetype == "python") {
        return new CmdRunProfile(view, "cd '%2'; /base/usr/bin/python3.2 '%3'");
    }
    if (filetype == "sh") {
        return new CmdRunProfile(view, "cd '%2'; /bin/sh '%3'");
    }
    return NULL;
}

RunProfile::RunProfile(View *view): QObject(view) {}

View *RunProfile::view() const
{
    return (View *) parent();
}
