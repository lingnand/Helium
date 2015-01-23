#include <src/BufferWorker.h>

BufferWorker::BufferWorker(QObject *parent): QThread(parent)
{
}

void BufferWorker::run()
{
    if (_work) {
        _work->run();
    }
}

BufferWorkPtr BufferWorker::work() const
{
    return _work;
}

void BufferWorker::setWork(BufferWorkPtr work)
{
    if (work != _work) {
        if (_work) {
            disconn(_work.get(), SIGNAL(inProgressChanged(float)),
                    this, SIGNAL(inProgressChanged(float)));
        }
        _work = work;
        if (_work) {
            conn(_work.get(), SIGNAL(inProgressChanged(float)),
                this, SIGNAL(inProgressChanged(float)));
        }
    }
}
