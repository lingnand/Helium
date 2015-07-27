/*
 * BufferStore.cpp
 *
 *  Created on: Jul 25, 2015
 *      Author: lingnan
 */

#include <BufferStore.h>
#include <Buffer.h>

BufferStore::BufferStore(QObject *parent): QObject(parent)
{
}

Buffer *BufferStore::newBuffer()
{
    return new Buffer(100, this);
}

Buffer *BufferStore::bufferForFilepath(const QString &filepath)
{
    for (int i = 0; i < children().size(); i++) {
        Buffer *b = (Buffer *) children()[i];
        if (b->filepath() == filepath) {
            return b;
        }
    }
    return NULL;
}
