/*
 * BufferHistory.h
 *
 *  Created on: Mar 29, 2015
 *      Author: lingnan
 */

#ifndef BUFFERHISTORY_H_
#define BUFFERHISTORY_H_

#include <src/BufferState.h>

class BufferHistory: public QObject, public QList<BufferState>
{
    Q_OBJECT
public:
    // anything equal or below 0 means no limitation on the size
    BufferHistory(int upperLimit = 0);
    virtual ~BufferHistory() {}
    // this will remove all items after current
    BufferState &copyCurrent();
    BufferState &current();
    bool advance();
    bool retract();
    bool advanceable();
    bool retractable();
Q_SIGNALS:
    void advanceableChanged(bool advanceable);
    void retractableChanged(bool retractable);
private:
    int _upperLimit;
    int _currentIndex;
};

#endif /* BUFFERHISTORY_H_ */
