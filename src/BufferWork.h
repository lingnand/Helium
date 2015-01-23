/*
 * BufferWork.h
 *
 *  Created on: Jan 19, 2015
 *      Author: lingnan
 */

#ifndef BUFFERWORK_H_
#define BUFFERWORK_H_

#include <QObject>
#include <boost/shared_ptr.hpp>

enum BufferWorkType { Save };

class BufferWork : public QObject
{
    Q_OBJECT
public:
    BufferWork(BufferWorkType type):_type(type) {}
    BufferWorkType type() const { return _type; }
    virtual ~BufferWork() {}
    virtual void run() = 0;
Q_SIGNALS:
    void inProgressChanged(float progress);
private:
    BufferWorkType _type;
};

typedef boost::shared_ptr<BufferWork> BufferWorkPtr;

#endif /* BUFFERWORK_H_ */
