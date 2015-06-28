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
#include <bb/system/SystemPrompt>
#include <bb/system/SystemUiPosition>
#include <bb/system/Clipboard>

static Utility *utility = NULL;

Utility::Utility():
    _toast(NULL), _bigToast(NULL), _dialog(NULL), _prompt(NULL)
{
}

bb::system::SystemToast *Utility::toast()
{
    if (!_toast) {
        _toast = new bb::system::SystemToast;
        _toast->setPosition(bb::system::SystemUiPosition::BottomCenter);
    } else {
        _toast->disconnect();
    }
    return _toast;
}

bb::system::SystemToast *Utility::bigToast()
{
    if (!_bigToast) {
        _bigToast = new bb::system::SystemToast;
        _bigToast->setPosition(bb::system::SystemUiPosition::MiddleCenter);
    }
    return _bigToast;
}

bb::system::SystemDialog *Utility::dialog()
{
    if (!_dialog) {
        _dialog = new bb::system::SystemDialog;
    } else {
        _dialog->disconnect();
    }
    return _dialog;
}

bb::system::SystemPrompt *Utility::prompt()
{
    if (!_prompt) {
        _prompt = new bb::system::SystemPrompt;
        conn(_prompt, SIGNAL(finished(bb::system::SystemUiResult::Type)),
            this, SLOT(onPromptFinished(bb::system::SystemUiResult::Type)));
    }
    disconnect(SIGNAL(promptFinished(bb::system::SystemUiResult::Type, const QString&)));
    return _prompt;
}

void Utility::toast(const QString &msg, const QString &label,
        const QObject *receiver, const char *method)
{
    if (!utility) {
        utility = new Utility;
    }
    bb::system::SystemToast *toast = utility->toast();
    toast->button()->setLabel(label);
    toast->setBody(msg);
    if (receiver && method) {
        conn(toast, SIGNAL(finished(bb::system::SystemUiResult::Type)), receiver, method);
    }
    toast->show();
}

void Utility::bigToast(const QString &msg)
{
    if (!utility) {
        utility = new Utility;
    }
    bb::system::SystemToast *toast = utility->bigToast();
    toast->setBody(msg);
    toast->show();
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
    Utility::dialog(confirm, QString(), QString(),
            title, body,
            receiver, method);
}

void Utility::dialog(const QString &confirm, const QString &cancel,
        const QString &title, const QString &body,
        const QObject *receiver, const char *method)
{
    Utility::dialog(confirm, cancel, QString(),
            title, body,
            receiver, method);
}

void Utility::dialog(const QString &confirm, const QString &cancel, const QString &custom,
        const QString &title, const QString &body,
        const QObject *receiver, const char *method)
{
    if (!utility) {
        utility = new Utility;
    }
    bb::system::SystemDialog *dialog = utility->dialog();
    dialog->confirmButton()->setLabel(confirm);
    dialog->cancelButton()->setLabel(cancel);
    dialog->customButton()->setLabel(custom);
    dialog->setTitle(title);
    dialog->setBody(body);
    if (receiver && method) {
        conn(dialog, SIGNAL(finished(bb::system::SystemUiResult::Type)), receiver, method);
    }
    dialog->show();
}

void Utility::prompt(const QString &confirm, const QString &cancel,
        const QString &title,
        const QString &defaultText, const QString &emptyText,
        const QObject *receiver, const char *method)
{
    if (!utility) {
        utility = new Utility;
    }
    bb::system::SystemPrompt *prompt = utility->prompt();
    prompt->confirmButton()->setLabel(confirm);
    prompt->cancelButton()->setLabel(cancel);
    prompt->setTitle(title);
    prompt->inputField()->setDefaultText(defaultText);
    prompt->inputField()->setEmptyText(emptyText);
    if (receiver && method) {
        conn(utility, SIGNAL(promptFinished(bb::system::SystemUiResult::Type, const QString&)), receiver, method);
    }
    prompt->show();
}

void Utility::onPromptFinished(bb::system::SystemUiResult::Type type)
{
    emit promptFinished(type, _prompt->inputFieldTextEntry());
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
            if (bb::cascades::AbstractTextControl *control =
                    dynamic_cast<bb::cascades::AbstractTextControl *>(editor->parent())) {
                control->loseFocus();
            }
            break;
    }
}
