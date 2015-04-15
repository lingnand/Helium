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
    *this = BufferLine();
}

BufferLine BufferLine::split(int position)
{
    BufferLine split;
    if (_size <= position) {
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
        if (_specialChars[i] == '&')
            output << "&amp;";
        else if (_specialChars[i] == '<')
            output << "&lt;";
        else if (_specialChars[i] == '>')
            output << "&gt;";
    }
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

int BufferState::cursorPosition() const
{
    return _cursorPosition;
}

void BufferState::setCursorPosition(int cursorPosition)
{
    _cursorPosition = cursorPosition;
}

BufferState::Position BufferState::focus(int cursorPosition) const
{
    Position pos;
    for (int i = 0; i < size(); i++)  {
        if (cursorPosition <= at(i).line.size()) {
            pos.lineIndex = i;
            pos.linePosition = cursorPosition;
            break;
        }
        // minus the newline character
        cursorPosition -= at(i).line.size() + 1;
    }
    return pos;
}

void BufferState::writePlainText(QTextStream &output) const
{
    if (empty())
       return;
    at(0).line.writePlainText(output);
    for (int i = 1; i < size(); i++) {
        output << '\n';
        at(i).line.writePlainText(output);
    }
}

QString BufferState::plainText() const
{
    QString text;
    QTextStream output(&text);
    writePlainText(output);
    output.flush();
    return text;
}

void BufferState::writeHighlightedHtml(QTextStream &output, const Range &range) const
{
    writeHighlightedHtml(output, range.from, range.to);
}

void BufferState::writeHighlightedHtml(QTextStream &output, int beginIndex, int endIndex) const
{
    if (empty())
        return;
    output << "<pre>";
    if (filetype().isEmpty()) {
        at(0).line.writePreText(output);
        for (int i = 1; i < size(); i++) {
            output << '\n';
            at(i).line.writePreText(output);
        }
    } else {
        beginIndex = qMax(0, beginIndex);
        endIndex = qMin(endIndex, size());
        int i = 0;
        for (; i < beginIndex; i++) {
            at(i).line.writePreText(output);
            output << '\n';
        }
        while (true) {
            if (!at(i).line.isEmpty()) {
                output << QString("<q id='%1'>").arg(i);
                output << at(i).highlightText;
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
            at(i).line.writePreText(output);
        }
    }
    output << "</pre>";
}

void BufferState::writeHighlightedHtml(QTextStream &output, int beginIndex) const
{
    writeHighlightedHtml(output, beginIndex, size());
}

QString BufferState::highlightedHtml(const Range &range) const
{
    return highlightedHtml(range.from, range.to);
}

QString BufferState::highlightedHtml(int beginIndex, int endIndex) const
{
    QString content;
    QTextStream stream(&content);
    writeHighlightedHtml(stream, beginIndex, endIndex);
    stream.flush();
    return content;
}

QString BufferState::highlightedHtml(int beginIndex) const
{
    return highlightedHtml(beginIndex, size());
}

QDebug operator<<(QDebug dbg, const BufferLine &line) {
    return dbg.nospace() << "{" << line.plainText() << "}";
}

QDebug operator<<(QDebug dbg, const BufferLineState &lineState) {
    return dbg.nospace() << "BufferLineState(" << lineState.line << ", " << lineState.highlightText << ")";
}

QDebug operator<<(QDebug dbg, const Range &range)
{
    return dbg.nospace() << "Range(from:" << range.from << ", to:" << range.to << ")";
}
