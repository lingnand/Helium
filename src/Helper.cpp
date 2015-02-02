/*
 * Helper.cpp
 *
 *  Created on: Nov 23, 2014
 *      Author: lingnan
 */

#include <src/Helper.h>
#include <bb/system/SystemToast>
#include <bb/system/SystemUiPosition>

void toast(const QString &msg)
{
    bb::system::SystemToast *toast = new bb::system::SystemToast;
    toast->setBody(msg);
    toast->setPosition(bb::system::SystemUiPosition::BottomCenter);
    toast->show();
}
