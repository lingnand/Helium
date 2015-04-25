/*
 * SignalBlocker.h
 *
 *  Created on: Apr 25, 2015
 *      Author: lingnan
 */

#ifndef SIGNALBLOCKER_H_
#define SIGNALBLOCKER_H_

class SignalBlocker
{
public:
    SignalBlocker(QObject *obj): _obj(obj), _old(obj->blockSignals(true)) {}
    ~SignalBlocker() {
        _obj->blockSignals(_old);
    }

private:
    QObject *_obj;
    bool _old;
};

#endif /* SIGNALBLOCKER_H_ */
