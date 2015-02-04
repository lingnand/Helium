/*
 * Helper.h
 *
 *  Created on: Nov 23, 2014
 *      Author: lingnan
 */

#ifndef HELPER_H_
#define HELPER_H_


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

void toast(const QString &msg);

void dialog(const QString &confirm,
        const QString &title, const QString &body,
        const QObject *receiver=NULL, const char *method=NULL);

void dialog(const QString &confirm, const QString &cancel,
        const QString &title, const QString &body,
        const QObject *receiver, const char *method);
#endif /* HELPER_H_ */
