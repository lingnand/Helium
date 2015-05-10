/*
 * Utility.h
 *
 *  Created on: Nov 23, 2014
 *      Author: lingnan
 */

#ifndef UTILITY_H_
#define UTILITY_H_

#include <QTextStream>

namespace bb {
    namespace cascades {
        class TextEditor;
        class KeyEvent;
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

class Utility {
public:
    static void toast(const QString &msg, const QString &label=QString(),
        const QObject *receiver=NULL, const char *method=NULL);
    static void escapeHtml(QTextStream &input, QTextStream &output);
    static void dialog(const QString &confirm,
            const QString &title, const QString &body,
            const QObject *receiver=NULL, const char *method=NULL);
    static void dialog(const QString &confirm, const QString &cancel,
            const QString &title, const QString &body,
            const QObject *receiver=NULL, const char *method=NULL);
    static void handleBasicTextControlModifiedKey(bb::cascades::TextEditor *, bb::cascades::KeyEvent *);
};

#endif /* UTILITY_H_ */
