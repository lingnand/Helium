/*
 * BufferHistory.cpp
 *
 *  Created on: Mar 29, 2015
 *      Author: lingnan
 */

#include <src/BufferHistory.h>

// BufferHistory

BufferHistory::BufferHistory(int upperLimit): _upperLimit(upperLimit), _currentIndex(0)
{
    append(BufferState());
}

BufferState &BufferHistory::copyCurrent()
{
    bool a = advanceable();
    bool r = retractable();
    while (size() > _currentIndex+1) {
        removeLast();
    }
    append(BufferState(last()));
    // take out of previous items (if necessary)
    if (_upperLimit > 0) {
        while (size() > _upperLimit) {
            removeFirst();
        }
    }
    _currentIndex = size() - 1;
    if (a)
        emit advanceableChanged(false);
    if (!r)
        emit retractableChanged(true);
    return current();
}

BufferState &BufferHistory::current()
{
    return operator[](_currentIndex);
}

bool BufferHistory::advance()
{
    if (_currentIndex == size() - 1)
        return false;
    _currentIndex++;
    if (_currentIndex == 1)
        emit retractableChanged(true);
    if (_currentIndex == size() - 1) {
        emit advanceableChanged(false);
    }
    return true;
}

bool BufferHistory::retract()
{
    if (_currentIndex == 0)
        return false;
    _currentIndex--;
    if (_currentIndex == size() - 2) {
        emit advanceableChanged(true);
    }
    if (_currentIndex == 0) {
        emit retractableChanged(false);
    }
    return true;
}

bool BufferHistory::advanceable()
{
    return _currentIndex < size()-1;
}

bool BufferHistory::retractable()
{
    return _currentIndex > 0;
}
