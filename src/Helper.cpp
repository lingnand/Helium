/*
 * Helper.cpp
 *
 *  Created on: Nov 23, 2014
 *      Author: lingnan
 */

#include <src/Helper.h>
#include <bb/system/SystemToast>
#include <bb/system/SystemDialog>
#include <bb/system/SystemUiPosition>

void toast(const QString &msg)
{
    bb::system::SystemToast *toast = new bb::system::SystemToast;
    toast->setBody(msg);
    toast->setPosition(bb::system::SystemUiPosition::BottomCenter);
    toast->show();
}

void dialogMod(bb::system::SystemDialog *dialog,
        const QString &title, const QString &body,
        const QObject *receiver=NULL, const char *method=NULL)
{
    dialog->setTitle(title);
    dialog->setBody(body);
    if (receiver && method)
        conn(dialog, SIGNAL(finished(bb::system::SystemUiResult::Type)), receiver, method);
    dialog->show();
}

void dialog(const QString &confirm,
        const QString &title, const QString &body,
        const QObject *receiver, const char *method)
{
    dialogMod(new bb::system::SystemDialog(confirm),
            title, body,
            receiver, method);
}

void dialog(const QString &confirm, const QString &cancel,
        const QString &title, const QString &body,
        const QObject *receiver, const char *method)
{
    dialogMod(new bb::system::SystemDialog(confirm, cancel),
            title, body,
            receiver, method);
}
