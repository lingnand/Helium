/*
 * ViewMode.cpp
 *
 *  Created on: May 3, 2015
 *      Author: lingnan
 */

#include <ViewMode.h>
#include <View.h>
#include <Utility.h>

ViewMode::ViewMode(View *view): QObject(view) {}

View *ViewMode::view() const
{
    return (View *) parent();
}
