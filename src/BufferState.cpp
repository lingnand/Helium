/*
 * BufferState.cpp
 *
 *  Created on: Mar 8, 2015
 *      Author: lingnan
 */

#include <src/BufferState.h>

// BufferLine

BufferLine::BufferLine():_size(0) {}

int BufferLine::size() const
{
    return _size;
}

bool BufferLine::isEmpty() const
{
    return _size == 0;
}

void BufferLine::clear()
{
    _size = 0;
    _preTextSegments.clear();
    _specialChars.clear();
    _highlightText.clear();
}

void BufferLine::setHighlightText(const QString &highlightText)
{
    _highlightText = highlightText;
}

HighlightStateDataPtr BufferLine::endHighlightState()
{
    return _endHighlightState;
}

void BufferLine::setEndHighlightState(HighlightStateDataPtr endState)
{
    _endHighlightState = endState;
}

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
    if (c == '&' || c == '<' || c == '>') {
        _specialChars.append(c);
    } else {
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

void BufferLine::writePlainText(QTextStream &output) const
{
    for (int i = 0; i < _preTextSegments.size(); i++) {
        output << _preTextSegments[i];
        if (i >= _specialChars.size())
            break;
        output << _specialChars[i];
    }
}

void BufferLine::writePreText(QTextStream &output) const
{
    for (int i = 0; i < _preTextSegments.size(); i++) {
        output << _preTextSegments[i];
        if (i >= _specialChars.size())
            break;
        if (_specialChars[i-1] == '&')
            output << "&amp";
        else if (_specialChars[i-1] == '<')
            output << "&lt;";
        else if (_specialChars[i-1] == '>')
            output << "&gt;";
    }
}

void BufferLine::writeHighlightText(QTextStream &output) const
{
    output << _highlightText;
}

QString BufferLine::plainText() const
{
    QString output;
    QTextStream stream(&output);
    writePlainText(stream);
    stream.flush();
    return output;
}

QString BufferLine::preText() const
{
    QString output;
    QTextStream stream(&output);
    writePreText(stream);
    stream.flush();
    return output;
}

QString BufferLine::highlightText() const
{
    return _highlightText;
}

// BufferState

BufferState::BufferState(): _cursorPosition(-1) {}

const QString &BufferState::filetype() const
{
    return _filetype;
}

void BufferState::setFiletype(const QString &filetype)
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

BufferState::Position BufferState::focus(int cursorPosition)
{
    Position pos;
    for (int i = 0; i < size(); i++)  {
        if (cursorPosition <= at(i).size()) {
            pos.lineIndex = i;
            pos.linePosition = cursorPosition;
            break;
        }
        // minus the newline character
        cursorPosition -= at(i).size() + 1;
    }
    return pos;
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
    output << "<pre>";
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
    output << "</pre>";
}

void BufferState::writeHighlightedHtml(QTextStream &output, int beginIndex)
{
    writeHighlightedHtml(output, beginIndex, size());
}

QString BufferState::highlightedHtml(int beginIndex, int endIndex)
{
    QString content;
    QTextStream stream(&content);
    writeHighlightedHtml(stream, beginIndex, endIndex);
    stream.flush();
    return content;
}

QString BufferState::highlightedHtml(int beginIndex)
{
    return highlightedHtml(0, size());
}

QDebug operator<<(QDebug dbg, const BufferLine &line) {
    return dbg.nospace() << "BufferLine(" << line.plainText() << ", " << line.highlightText() << ")";
}
