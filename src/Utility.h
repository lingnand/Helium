/*
 * Utility.h
 *
 *  Created on: Nov 23, 2014
 *      Author: lingnan
 */

#ifndef UTILITY_H_
#define UTILITY_H_

#include <QTextStream>
#include <bb/system/SystemUiResult>

namespace bb {
    namespace cascades {
        class TextEditor;
        class KeyEvent;
    }
    namespace system {
        class SystemToast;
        class SystemDialog;
        class SystemPrompt;
    }
}

#define conn(sender, signal, receiver, slot) { \
    bool res = ::QObject::connect(sender, signal, receiver, slot); \
    Q_ASSERT(res); \
    Q_UNUSED(res); \
}

#define disconn(sender, signal, receiver, slot) { \
    bool res = ::QObject::disconnect(sender, signal, receiver, slot); \
    Q_ASSERT(res); \
    Q_UNUSED(res); \
}

#define PMOD(x, y) (((x) % (y) + (y)) % (y))

class Utility : public QObject {
    Q_OBJECT
public:
    static void toast(const QString &msg, const QString &label=QString(),
        const QObject *receiver=NULL, const char *method=NULL);
    static void bigToast(const QString &msg);
    static void escapeHtml(QTextStream &input, QTextStream &output);
    static void dialog(const QString &confirm,
            const QString &title, const QString &body,
            const QObject *receiver=NULL, const char *method=NULL);
    static void dialog(const QString &confirm, const QString &cancel,
            const QString &title, const QString &body,
            const QObject *receiver=NULL, const char *method=NULL);
    static void dialog(const QString &confirm, const QString &cancel, const QString &custom,
            const QString &title, const QString &body,
            const QObject *receiver=NULL, const char *method=NULL);
    static void prompt(const QString &confirm, const QString &cancel,
        const QString &title, const QString &body,
        const QString &defaultText, const QString &emptyText,
        const QObject *receiver=NULL, const char *method=NULL);
    static void handleBasicTextControlModifiedKey(bb::cascades::TextEditor *, bb::cascades::KeyEvent *);
Q_SIGNALS:
    void promptFinished(bb::system::SystemUiResult::Type, const QString &);
private:
    Utility();
    bb::system::SystemToast *_toast;
    bb::system::SystemToast *toast();
    bb::system::SystemToast *_bigToast;
    bb::system::SystemToast *bigToast();
    bb::system::SystemDialog *_dialog;
    bb::system::SystemDialog *dialog();
    bb::system::SystemPrompt *_prompt;
    bb::system::SystemPrompt *prompt();
    Q_SLOT void onPromptFinished(bb::system::SystemUiResult::Type);
};

#endif /* UTILITY_H_ */
