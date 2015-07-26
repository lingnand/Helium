/*
 * BufferStore.cpp
 *
 *  Created on: Jul 25, 2015
 *      Author: lingnan
 */

#include <BufferStore.h>
#include <Buffer.h>

BufferStore::BufferStore()
{
}

BufferStore::~BufferStore()
{
    for (int i = 0; i < _buffers.size(); i++)
        _buffers[i]->deleteLater();
}

Buffer *BufferStore::newBuffer()
{
    Buffer *b = new Buffer(100);
    _buffers.append(b);
    return b;
}

Buffer *BufferStore::bufferForFilepath(const QString &filepath)
{
    for (int i = 0; i < _buffers.size(); i++) {
        if (_buffers[i]->filepath() == filepath) {
            return _buffers[i];
        }
    }
    return NULL;
}

void BufferStore::remove(Buffer *buffer)
{
    if (_buffers.removeOne(buffer)) {
        buffer->deleteLater();
    }
}
