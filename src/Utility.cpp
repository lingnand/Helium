/*
 * Utility.cpp
 *
 *  Created on: Nov 23, 2014
 *      Author: lingnan
 */

#include <bb/cascades/AbstractTextControl>
#include <bb/cascades/TextEditor>
#include <bb/cascades/KeyEvent>
#include <bb/system/SystemToast>
#include <bb/system/SystemDialog>
#include <bb/system/SystemPrompt>
#include <bb/system/SystemUiPosition>
#include <bb/system/Clipboard>
#include <srchilite/sourcehighlight.h>
#include <srchilite/formattermanager.h>
#include <Helium.h>
#include <AppearanceSettings.h>
#include <Utility.h>

static Utility *utility = NULL;

void Utility::connect(const char *signal, const QObject *receiver, const char *method,
        Qt::ConnectionType type)
{
    if (!utility) {
        utility = new Utility;
    }
    bool res = QObject::connect(utility, signal, receiver, method, type);
    Q_ASSERT(res);
    Q_UNUSED(res);
}

void Utility::disconnect(const char *signal, const QObject *receiver, const char *method)
{
    if (!utility) {
        return;
    }
    bool res = QObject::disconnect(utility, signal, receiver, method);
    Q_ASSERT(res);
    Q_UNUSED(res);
}

Utility::Utility():
    _toast(NULL), _bigToast(NULL), _dialog(NULL), _prompt(NULL),
    _sourceHighlight(NULL)
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
        } else if (ch == '>') {
            output << "&gt;";
        } else {
            output << ch;
        }
    }
}

srchilite::SourceHighlight *Utility::sourceHighlight()
{
    if (!_sourceHighlight) {
        _sourceHighlight = new srchilite::SourceHighlight("xhtml.outlang");
        // setup the connections with the global instance
        AppearanceSettings *appearance = Helium::instance()->appearance();
        _sourceHighlight->setStyleFile(appearance->highlightStyleFile().toStdString());
        conn(appearance, SIGNAL(highlightStyleFileChanged(const QString&)),
            this, SLOT(onHighlightStyleFileChanged(const QString&)));
    }
    return _sourceHighlight;
}

void Utility::onHighlightStyleFileChanged(const QString &styleFile)
{
    if (_sourceHighlight) {
        _sourceHighlight->setStyleFile(styleFile.toStdString());
        emit htmlFormatterStyleChanged();
    }
}

void Utility::formatHtml(const QString &elem, const QString &input, QTextStream &output)
{
    if (!utility) {
        utility = new Utility;
    }
    srchilite::SourceHighlight *highlighter = utility->sourceHighlight();
    srchilite::FormatterPtr formatter;
    if (elem.isNull())
        formatter = highlighter->getFormatterManager()->getDefaultFormatter();
    else
        formatter = highlighter->getFormatterManager()->getFormatter(elem.toStdString());
    formatter->format(input.toStdString());
    output << QString::fromStdString(highlighter->getBuffer().str());
    highlighter->clearBuffer();
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
        const QString &title, const QString &body,
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
    prompt->setBody(body);
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

QString Utility::shortenPath(const QString &path)
{
    QString shorten = path;
    if (path.startsWith("/accounts/1000/removable/sdcard")) {
        QStringRef rem = path.rightRef(path.size()-31); // XXX: hardcoded length
        shorten = "SD";
        if (!rem.isEmpty()) {
            shorten += ":";
            shorten += rem;
        }
    } else if (path.startsWith("/accounts/1000/shared/")) {
        shorten = path.right(path.size()-22); // XXX: hardcoded length
    }
    return shorten;
}
