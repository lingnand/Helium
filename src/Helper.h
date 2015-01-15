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

#endif /* HELPER_H_ */
