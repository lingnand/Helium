/*
 * SaveWork.h
 *
 *  Created on: Jan 19, 2015
 *      Author: lingnan
 */

#ifndef SAVEWORK_H_
#define SAVEWORK_H_

#include <src/BufferWork.h>
#include <src/BufferState.h>

class SaveWork : public BufferWork
{
    Q_OBJECT
public:
    SaveWork();
    void setState(const BufferState &state);
    void run();
protected:
    BufferState _state;
};

typedef boost::shared_ptr<BufferWork> SaveWorkPtr;

#endif /* SAVEWORK_H_ */
