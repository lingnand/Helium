/*
 * BufferStore.h
 *
 *  Created on: Jul 25, 2015
 *      Author: lingnan
 */

#ifndef BUFFERSTORE_H_
#define BUFFERSTORE_H_

class Buffer;

class BufferStore : public QObject
{
    Q_OBJECT
public:
    BufferStore(QObject *parent=NULL);
    virtual ~BufferStore() {}
    Buffer *newBuffer();
    Buffer *bufferForFilepath(const QString &filepath);
};

#endif /* BUFFERSTORE_H_ */
