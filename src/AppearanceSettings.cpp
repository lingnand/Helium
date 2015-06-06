/*
 * AppearanceSettings.cpp
 *
 *  Created on: Jun 6, 2015
 *      Author: lingnan
 */

#include <AppearanceSettings.h>

AppearanceSettings::AppearanceSettings(bool hideActionBar, QObject *parent):
    QObject(parent),
    _hideActionBar(hideActionBar)
{}

void AppearanceSettings::setHideActionBar(bool hideActionBar)
{
    if (hideActionBar != _hideActionBar) {
        _hideActionBar = hideActionBar;
        emit hideActionBarChanged(_hideActionBar);
    }
}
