/*
 * Utility.cpp
 *
 *  Created on: Nov 23, 2014
 *      Author: lingnan
 */

#include <Utility.h>
#include <bb/cascades/AbstractTextControl>
#include <bb/cascades/TextEditor>
#include <bb/cascades/KeyEvent>
#include <bb/system/SystemToast>
#include <bb/system/SystemDialog>
#include <bb/system/SystemUiPosition>
#include <bb/system/Clipboard>

static bb::system::SystemToast *_toast = NULL;
static bb::system::SystemDialog *_dialog = NULL;

void Utility::toast(const QString &msg, const QString &label,
        const QObject *receiver, const char *method)
{
    if (!_toast) {
        _toast = new bb::system::SystemToast;
        _toast->setPosition(bb::system::SystemUiPosition::BottomCenter);
        _toast->button()->setLabel(label);
    } else {
        _toast->disconnect();
    }
    _toast->setBody(msg);
    if (receiver && method) {
        conn(_toast, SIGNAL(finished(bb::system::SystemUiResult::Type)), receiver, method);
    }
    _toast->show();
}

void Utility::escapeHtml(QTextStream &input, QTextStream &output)
{
    QChar ch;
    while (!input.atEnd()) {
        input >> ch;
        if (ch == '&') {
            output << "&amp;";
        } else if (ch == '<') {
            output << "&lt;";
        } else {
            output << ch;
        }
    }
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

void Utility::handleBasicTextControlModifiedKey(bb::cascades::TextEditor *editor, bb::cascades::KeyEvent *event)
{
    switch (event->keycap()) {
        case KEYCODE_V: {
            bb::system::Clipboard clipboard;
            QString paste = clipboard.value("text/plain");
            editor->insertPlainText(paste);
            break;
        }
        case KEYCODE_SPACE:
            // TODO: this is kinda of a hack
            ((bb::cascades::AbstractTextControl *) editor->parent())->loseFocus();
            break;
    }
}
