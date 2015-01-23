#ifndef BUFFERWORKER_H_
#define BUFFERWORKER_H_

#include <QObject>
#include <QThread>
#include <src/BufferWork.h>

class BufferWorker : public QThread
{
    Q_OBJECT
public:
    BufferWorker(QObject *parent = 0);
    virtual ~BufferWorker() {}
    void run();
    BufferWorkPtr work() const;
    void setWork(BufferWorkPtr work);
Q_SIGNALS:
    void inProgressChanged(float progress);
private:
    BufferWorkPtr _work;
};

#endif /* BUFFERWORKER_H_ */
