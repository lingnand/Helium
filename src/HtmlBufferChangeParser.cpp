#include <HtmlBufferChangeParser.h>

HtmlBufferChangeParser::HtmlBufferChangeParser(): _lastDelayable(false)
{
}

bool delayableInStructure(const BufferStateChange &change, int cursorLine)
{
    int diff = -1;
    for (int i = 0; i < change.size(); i++) {
        if (i > cursorLine && diff < 0) // not a single context line
            return false;
        if (change[i].index < 0) {
            if (!change[i].line.isEmpty()) {
                return false;
            }
        } else {
            if (diff < 0) {
                diff = change[i].index - i;
            } else if (diff != change[i].index - i) { // not in order
                return false;
            }
        }
    }
    return true;
}

BufferStateChange HtmlBufferChangeParser::parseBufferChange(const QString &input, ParserPosition position, int cursorPosition)
{
    _stopParsing = false;
    _cursorPosition = cursorPosition;
    _cursorLine = -1;
    _change = BufferStateChange();
    qDebug() << ">>>>>> start parsing >>>>>>";
    parse(input, position);
    assert(_cursorLine >= 0);
    _change._delayable = _change._delayable && delayableInStructure(_change, _cursorLine);
    // remove unneeded lines
    if (!_change.atEnd()) {
        while (_change.size() > _cursorLine+1 && _change.last().index > 0 && _change.last().index == _change[_change.size()-2].index+1) {
            _change.removeLast();
        }
    }
    qDebug() << ">>>>>> end parsing >>>>>>";
    _lastDelayable = _change._delayable;
    return _change;
}

void HtmlBufferChangeParser::parseCharacter(const QChar &ch, int charCount)
{
//    qDebug() << "getting" << ch;
    bool shouldChangeLine = ch == '\n' || ch == '\r';
    if (shouldChangeLine) {
        qDebug() << "encountered newline, charCount:" << charCount;
         // make sure we have at least one *valid* context line *after* the cursor line
        if (_cursorLine >= 0 &&
                _change.size() > _cursorLine+1 && _change.last().index >= 0) {
            _stopParsing = true;
            return;
        }
        // we always append new lines (let the <q> tag clear unuseful lines out!)
        _change.append(ChangedBufferLine());
    } else if (!ch.isNull()) {
        _change.last().line << ch;
    }
    if (charCount == _cursorPosition) {
        _cursorLine = _change.size() - 1;
        qDebug() << "reached cursor, charcount:" << charCount << "ch:" << ch;
        qDebug() << "shouldChangeLine:" << shouldChangeLine;
        qDebug() << "isLetterOrNumber:" << ch.isLetterOrNumber();
        qDebug() << "isSpace:" << ch.isSpace();
        qDebug() << "_lastDelayable:" << _lastDelayable;
        // delay highlight if
        // 1. the character is a letter or number
        // 2. last time we didn't 'delay' highlighting - meaning there is no prediction currently
        //    we need to delay highlighting if we know a given character IS going to bring up prediction
        //    - in this case, we assume all white space characters can
        //    TODO: devise a strategy to reliably tell if there is prediction prompt
        _change._delayable =  _change._delayable && !shouldChangeLine && (ch.isLetterOrNumber() || (ch.isSpace() && !_lastDelayable));
    }
}

void HtmlBufferChangeParser::parseTag(const QString &name, const QString &attributeName, const QString &attributeValue)
{
    if (name == "q") {
        Q_ASSERT(attributeName == "id" && !attributeValue.isEmpty());
        // after seeing q we can throw away all the previous lines
        // (when we haven't reached cursor yet)
        int index = attributeValue.toInt();
        if (_cursorLine < 0) {
            while (_change.size() > 1)
                _change.removeFirst();
            _change._startIndex = index;
            qDebug() << "setting startIndex to:" << _change.startIndex();
        } else if (_change._delayable && _change.size() == _cursorLine+1) {
            // reading a q tag after the cursor in the same line:
            // this means there has just been a deletion that joins two lines
            // together
            _change._delayable = false;
        }
        _change.last().index = index;
    }
}

bool HtmlBufferChangeParser::stopParsing() { return _stopParsing; }
void HtmlBufferChangeParser::reachedEnd() { _change._atEnd = true; }

QDebug operator<<(QDebug dbg, const ChangedBufferLine &line)
{
    dbg.nospace() << "ChangedBufferLine(index:" << line.index << ", " << line.line << ")";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const BufferStateChange &change)
{
    dbg.nospace() << "Change(delayable:" << change.delayable() <<
            ", atEnd:" << change.atEnd() <<
            ", startIndex:" << change.startIndex() <<
            ", " << QList<ChangedBufferLine>(change) << ")";
    return dbg.space();
}
