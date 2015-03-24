/*
 * BufferState.cpp
 *
 *  Created on: Mar 8, 2015
 *      Author: lingnan
 */

#include <src/BufferState.h>

// BufferLine

BufferLine::BufferLine():_size(0) {}

int BufferLine::size()
{
    return _size;
}

bool BufferLine::isEmpty()
{
    return _size == 0;
}

void BufferLine::setHighlightText(const QString &highlightText)
{
    _highlightText = highlightText;
}

HighlightStateDataPtr BufferLine::beginHighlightState()
{
    return _beginHighlightState;
}

HighlightStateDataPtr BufferLine::endHighlightState()
{
    return _endHighlightState;
}

void BufferLine::setEndHighlightState(HighlightStateDataPtr endState)
{
    _endHighlightState = endState;
}

// TODO need to think about the edge cases!
BufferLine BufferLine::split(int position)
{
    BufferLine split;
    if (_size <= position) {
        split._endHighlightState = _endHighlightState;
        return split;
    }
    _size -= position;
    split._size = position;
    while (position > 0) {
        QString elem = _preTextSegments.takeFirst();
        split._preTextSegments.append(elem.left(position));
        position -= elem.size();
        if (position <= 0) {
            _preTextSegments.prepend(elem.right(-position));
            break;
        }
        split._specialChars.append(_specialChars.takeFirst());
        position--;
    }
    swap(split);
    return split;
}

void BufferLine::append(const QChar &c)
{
    if (_preTextSegments.size() == _specialChars.size())
        _preTextSegments.append("");
    switch (c) {
        case '&': case '<': case '>':
            _specialChars.append(c);
            break;
        default:
            _preTextSegments.last().append(c);
    }
    _size++;
}

void BufferLine::append(const QString &str)
{
    for (int i = 0; i < str.size(); i++) {
        append(str[i]);
    }
}

void BufferLine::append(const BufferLine &other)
{
    if (other._size == 0)
        return;
    if (_preTextSegments.size() == _specialChars.size())
        _preTextSegments.append(other._preTextSegments);
    else {
        QList<QString> seg = other._preTextSegments;
        _preTextSegments.last().append(seg.takeFirst());
        _preTextSegments.append(seg);
    }
    _size += other._size;
    _specialChars.append(other._specialChars);
}

BufferLine &BufferLine::operator<<(const QChar &c) { append(c); return *this; }
BufferLine &BufferLine::operator<<(const QString &str) { append(str); return *this; }
BufferLine &BufferLine::operator<<(const BufferLine &line) { append(line); return *this; }

void BufferLine::swap(BufferLine &other)
{
    BufferLine temp(*this);
    operator=(other);
    other = temp;
}

void BufferLine::writePlainText(QTextStream &output)
{
    for (int i = 0; i < _preTextSegments.size(); i++) {
        output << _preTextSegments[i];
        if (i >= _specialChars.size())
            break;
        output << _specialChars[i];
    }
}

void BufferLine::writePreText(QTextStream &output)
{
    for (int i = 0; i < _preTextSegments.size(); i++) {
        output << _preTextSegments[i];
        if (i >= _specialChars.size())
            break;
        switch (_specialChars[i-1]) {
            case '&': output << "&amp;"; break;
            case '<': output << "&lt;"; break;
            case '>': output << "&gt;"; break;
        }
    }
}

void BufferLine::writeHighlightText(QTextStream &output)
{
    output << _highlightText;
}

// BufferState

BufferState::BufferState(): _cursorPosition(-1) {}

QString &BufferState::filetype()
{
    return _filetype;
}

void BufferState::setFiletype(QString &filetype)
{
    _filetype = filetype;
}

int BufferState::cursorPosition()
{
    return _cursorPosition;
}

void BufferState::setCursorPosition(int cursorPosition)
{
    _cursorPosition = cursorPosition;
}

int BufferState::focus(int cursorPosition)
{
    for (int i = 0; i < size(); i++)  {
        cursorPosition -= at(i).size();
        if (cursorPosition <= 0)
            return i;
        // minus the newline character
        cursorPosition--;
    }
    return -1;
}

void BufferState::writePlainText(QTextStream &output)
{
    if (empty())
       return;
    at(0).writePlainText(output);
    for (int i = 1; i < size(); i++) {
        output << '\n';
        at(i).writePlainText(output);
    }
}

void BufferState::writeHighlightedHtml(QTextStream &output, int beginIndex, int endIndex)
{
    if (empty())
        return;
    if (filetype().isEmpty()) {
        at(0).writePreText(output);
        for (int i = 1; i < size(); i++) {
            output << '\n';
            at(i).writePreText(output);
        }
    } else {
        beginIndex = qMax(0, beginIndex);
        endIndex = qMin(endIndex, size());
        int i = 0;
        for (; i < beginIndex; i++) {
            at(i).writePreText(output);
            output << '\n';
        }
        while (true) {
            if (!at(i).isEmpty()) {
                output << QString("<q id='%1'>").arg(i);
                at(i).writeHighlightText(output);
                output << "</q>";
            }
            i++;
            if (i == endIndex) {
                break;
            }
            output << '\n';
        }
        for (; i < size(); i++) {
            output << '\n';
            at(i).writePreText(output);
        }
    }
}

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
    return at(_currentIndex);
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
    if (currentIndex == 0)
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
