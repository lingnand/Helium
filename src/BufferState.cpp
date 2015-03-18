/*
 * BufferState.cpp
 *
 *  Created on: Mar 8, 2015
 *      Author: lingnan
 */

#include <src/BufferState.h>

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

QString &BufferState::filetype()
{
    return _filetype;
}

void BufferState::setFiletype(QString &filetype)
{
    _filetype = filetype;
}

int BufferState::focusedLine(int cursorPosition)
{
    if (empty())
        return -1;
    int i = 0;
    while (i < size())  {
        cursorPosition -= at(i).charCount();
        if (cursorPosition <= 0)
            return i;
        i++;
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

// this uses the same syntax as common substring function - the end line
void BufferState::writePartialHighlightedHtml(QTextStream &output, int beginIndex, int endIndex)
{
    if (empty())
        return;
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
