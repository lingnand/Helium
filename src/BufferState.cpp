/*
 * BufferState.cpp
 *
 *  Created on: Mar 8, 2015
 *      Author: lingnan
 */

#include <src/BufferState.h>

// BufferLine

void BufferLine::BufferLine():_charCount(0)
{
}

int BufferLine::charCount()
{
    return _charCount;
}

bool BufferLine::isEmpty()
{
    return _preTextSegments.empty() && _specialChars.empty();
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

BufferLine &BufferLine::operator<<(QChar c)
{
    switch (c) {
        case '&': case '<': case '>':
            _specialChars.append(c);
            _preTextSegments.append("");
            break;
        default:
            if (_preTextSegments.empty()) {
                _preTextSegments.append("");
            }
            // append to the last segment
            _preTextSegments.last().append(c);
    }
    _charCount++;
    return *this;
}

void BufferLine::writePlainText(QTextStream &output)
{
    if (_preTextSegments.empty())
        return;
    output << _preTextSegments[0];
    for (int i = 1; i < _preTextSegments.size(); i++) {
        output << _specialChars[i-1];
        output << _preTextSegments[i];
    }
}

void BufferLine::writePreText(QTextStream &output)
{
    if (_preTextSegments.empty())
        return;
    output << _preTextSegments[0];
    // replace all the special characters
    for (int i = 1; i < _preTextSegments.size(); i++) {
        switch (_specialChars[i-1]) {
            case '&': output << "&amp;"; break;
            case '<': output << "&lt;"; break;
            case '>': output << "&gt;"; break;
        }
        output << _preTextSegments[i];
    }
}

void BufferLine::writeHighlightText(QTextStream &output)
{
    output << _highlightText;
}

// BufferState

BufferState::BufferState(): _cursorPosition(-1)
{
}

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
    if (empty())
        return -1;
    for (int i = 0; i < size(); i++)  {
        cursorPosition -= at(i).charCount();
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
