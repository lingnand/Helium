/*
 * BufferStore.h
 *
 *  Created on: Jul 25, 2015
 *      Author: lingnan
 */

#ifndef BUFFERSTORE_H_
#define BUFFERSTORE_H_

class Buffer;

class BufferStore
{
public:
    BufferStore();
    virtual ~BufferStore();
    Buffer *newBuffer();
    Buffer *bufferForFilepath(const QString &filepath);
    void remove(Buffer *);
private:
    QList<Buffer *> _buffers;
};

#endif /* BUFFERSTORE_H_ */
