/*
 * BufferState.cpp
 *
 *  Created on: Mar 8, 2015
 *      Author: lingnan
 */

#include <BufferState.h>
#include <Filetype.h>
#include <Helium.h>

// BufferLine

BufferLine::BufferLine(): _1size(0), _4size(0), _5size(0) {}

int BufferLine::size() const
{
    return _1size + _4size + _5size;
}

int BufferLine::preTextSize() const
{
    return _1size + 4*_4size + 5*_5size;
}

bool BufferLine::isEmpty() const
{
    return _preTextSegments.empty() && _specialChars.empty();
}

void BufferLine::clear()
{
    *this = BufferLine();
}

BufferLine BufferLine::split(int position)
{
    BufferLine split;
    if (size() <= position) {
        return split;
    }
    while (position > 0) {
        QString elem = _preTextSegments.takeFirst();
        QString transfer = elem.left(position);
        split._preTextSegments.append(transfer);
        _1size -= transfer.size();
        split._1size += transfer.size();
        position -= elem.size();
        if (position <= 0) {
            _preTextSegments.prepend(elem.right(-position));
            break;
        }
        QChar spTransfer = _specialChars.takeFirst();
        split._specialChars.append(spTransfer);
        if (spTransfer == '&') {
            _5size--;
            split._5size++;
        } else if (spTransfer == '<') {
            _4size--;
            split._4size++;
        } else { // '>'
            _1size--;
            split._1size++;
        }
        position--;
    }
    swap(split);
    return split;
}

void BufferLine::append(const QChar &c)
{
    switch (c.unicode()) {
        // handle exceptions in Cc
        case 0x0001:
        case 0x0002:
        case 0x0003:
        case 0x0004:
        case 0x0005:
        case 0x0006:
        case 0x0007:
        case 0x0008:
        case 0x000b:
        case 0x000c:
        case 0x000e:
        case 0x000f:
        case 0x0010:
        case 0x0011:
        case 0x0012:
        case 0x0013:
        case 0x0014:
        case 0x0015:
        case 0x0016:
        case 0x0017:
        case 0x0018:
        case 0x0019:
        case 0x001a:
        case 0x001b:
        case 0x001c:
        case 0x001d:
        case 0x001e:
        case 0x001f:
            return;
        default:
            // check if the character is one of these values
            if (_preTextSegments.size() == _specialChars.size())
                _preTextSegments.append("");
            if (c == '&') {
                _specialChars.append(c);
                _5size++;
            } else if (c == '<') {
                _specialChars.append(c);
                _4size++;
            } else if (c == '>') {
                _specialChars.append(c);
                _1size++;
            } else {
                _preTextSegments.last().append(c);
                _1size++;
            }
    }
}

void BufferLine::append(const QString &str)
{
    for (int i = 0; i < str.size(); i++) {
        append(str[i]);
    }
}

void BufferLine::append(const BufferLine &other)
{
    if (other.isEmpty())
        return;
    if (_preTextSegments.size() == _specialChars.size())
        _preTextSegments.append(other._preTextSegments);
    else {
        QList<QString> seg = other._preTextSegments;
        _preTextSegments.last().append(seg.takeFirst());
        _preTextSegments.append(seg);
    }
    _specialChars.append(other._specialChars);
    _1size += other._1size;
    _4size += other._4size;
    _5size += other._5size;
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
        else // '>'
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

int BufferState::cursorPositionAtLine(int line) const
{
    int cpos = 0;
    for (int i = 0; i < line; i++) {
        cpos += at(i).line.size() + 1;
    }
    return cpos;
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

QString BufferState::plainText() const
{
    QString text;
    if (empty())
       return text;
    QTextStream output(&text);
    at(0).line.writePlainText(output);
    for (int i = 1; i < size(); i++) {
        output << '\n';
        at(i).line.writePlainText(output);
    }
    output.flush();
    return text;
}

int BufferState::plainTextSize() const
{
    if (empty())
        return 0;
    int total = at(0).line.size();
    for (int i = 1; i < size(); i++) {
        total += 1 + at(i).line.size();
    }
    return total;
}

void BufferState::writePlainText(QTextStream &output) const
{
    if (!empty()) {
        at(0).line.writePlainText(output);
        for (int i = 1; i < size(); i++) {
            output << '\n';
            at(i).line.writePlainText(output);
        }
    }
}

ParserPosition BufferState::writeHighlightedHtml(QTextStream &output, const Range &range) const
{
    return writeHighlightedHtml(output, range.from, range.to);
}

ParserPosition BufferState::writeHighlightedHtml(QTextStream &output, int beginIndex) const
{
    return writeHighlightedHtml(output, beginIndex, size());
}

ParserPosition BufferState::writeHighlightedHtml(QTextStream &output, int beginIndex, int endIndex) const
{
    ParserPosition pos;
    if (empty())
        return pos;
    output << "<pre>";
    if (_highlightType.shouldHighlight()) {
        beginIndex = qMax(0, beginIndex);
        endIndex = qMin(endIndex, size());
        int i = 0;
        bool writtenPlainText = false;
        for (; i < beginIndex; i++) {
//            qDebug() << "writing line" << i << "...";
            const BufferLine &line = at(i).line;
            line.writePreText(output);
            pos.charCount += line.size();
            pos.htmlCount += line.preTextSize();
//            qDebug() << "updated position to" << pos;
            output << '\n';
            pos.charCount++;
            pos.htmlCount++;
            writtenPlainText = true;
        }
        if (writtenPlainText) {
            // nudge the htmlCount by 6
            // - one is for reading from the start of the next line
            //   this is for the reason that we'd like
            //   the parse to start parsing from the html line
            //   NOTE: we assume that as long as we've written plainText
            //   then cursorPosition != charCount
            //   i.e. the range should be at least 1 line away from the cursorLine
            // - remaining 5 is for <pre> at the start
//            qDebug() << "nudging htmlCount by 6...";
            pos.htmlCount += 6;
//            qDebug() << "updated position to" << pos;
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
    } else {
        at(0).line.writePreText(output);
        for (int i = 1; i < size(); i++) {
            output << '\n';
            at(i).line.writePreText(output);
        }
    }
    output << "</pre>";
    return pos;
}

QDebug operator<<(QDebug dbg, const BufferLine &line) {
    dbg.nospace() << "{" << line.plainText() << "}";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const BufferLineState &lineState) {
    dbg.nospace() << "BufferLineState(" << lineState.line << ", " << lineState.highlightText << ")";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const Range &range)
{
    dbg.nospace() << "Range(from:" << range.from << ", to:" << range.to << ")";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const BufferState::Position &position)
{
    dbg.nospace() << "BufferStatePosition(line:" << position.lineIndex << ", pos:" << position.linePosition << ")";
    return dbg.space();
}
