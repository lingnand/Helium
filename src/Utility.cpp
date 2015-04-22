/*
 * Utility.cpp
 *
 *  Created on: Nov 23, 2014
 *      Author: lingnan
 */

#include <Utility.h>
#include <bb/system/SystemToast>
#include <bb/system/SystemDialog>
#include <bb/system/SystemUiPosition>

static bb::system::SystemToast *_toast = NULL;
static bb::system::SystemDialog *_dialog = NULL;

void Utility::toast(const QString &msg)
{
    if (!_toast) {
        _toast = new bb::system::SystemToast;
        _toast->setPosition(bb::system::SystemUiPosition::BottomCenter);
    }
    _toast->setBody(msg);
    _toast->show();
}

void Utility::dialog(const QString &confirm,
        const QString &title, const QString &body,
        const QObject *receiver, const char *method)
{
    Utility::dialog(confirm, QString(),
            title, body,
            receiver, method);
}

void Utility::dialog(const QString &confirm, const QString &cancel,
        const QString &title, const QString &body,
        const QObject *receiver, const char *method)
{
    if (!_dialog) {
        _dialog = new bb::system::SystemDialog;
    } else {
        _dialog->disconnect();
    }
    _dialog->confirmButton()->setLabel(confirm);
    _dialog->cancelButton()->setLabel(cancel);
    _dialog->setTitle(title);
    _dialog->setBody(body);
    if (receiver && method) {
        conn(_dialog, SIGNAL(finished(bb::system::SystemUiResult::Type)), receiver, method);
    }
    _dialog->show();
}
