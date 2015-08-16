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

namespace srchilite {
    class SourceHighlight;
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
#define DEFAULT_PROJECT_PATH "/accounts/1000/shared/documents"

class Utility : public QObject {
    Q_OBJECT
public:
    static void toast(const QString &msg, const QString &label=QString(),
        const QObject *receiver=NULL, const char *method=NULL);
    static void bigToast(const QString &msg);
    static void escapeHtml(QTextStream &input, QTextStream &output);
    // a thread unsafe helper that formats the given input and appends to the output
    static void formatHtml(const QString &elem, const QString &input, QTextStream &output);
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
    static QString shortenPath(const QString &path);
    static void connect(const char *signal, const QObject *receiver, const char *method,
            Qt::ConnectionType type = Qt::AutoConnection);
    static void disconnect(const char *signal=0, const QObject *receiver=0, const char *method=0);
Q_SIGNALS:
    void promptFinished(bb::system::SystemUiResult::Type, const QString &);
    void htmlFormatterStyleChanged();
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
    srchilite::SourceHighlight *_sourceHighlight;
    srchilite::SourceHighlight *sourceHighlight();

    Q_SLOT void onPromptFinished(bb::system::SystemUiResult::Type);
    Q_SLOT void onHighlightStyleFileChanged(const QString &);
};

#endif /* UTILITY_H_ */
