/*
 * RunProfile.cpp
 *
 *  Created on: May 9, 2015
 *      Author: lingnan
 */

#include <RunProfile.h>
#include <View.h>

RunProfile::RunProfile(View *view): QObject(view) {}

View *RunProfile::view() const
{
    return (View *) parent();
}
