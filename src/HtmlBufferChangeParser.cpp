#include <src/HtmlBufferChangeParser.h>

HtmlBufferChangeParser::HtmlBufferChangeParser(): _delayable(false)
{
}

BufferStateChange HtmlBufferChangeParser::parseBufferChange(QTextStream &input, int cursorPosition)
{
    _startParsing = true;
    _stopParsing = false;
    _cursorLine = -1;
    _cursorPosition = cursorPosition;
    _change = BufferStateChange();
    qDebug() << ">>>>>> start parsing >>>>>>";
    parse(input);
    // remove everything after the cursorLine, if the line after cursorLine follows cursorLine immediately
    // NOTE: we assume that only the cursor line can assume an endIndex that's different from the startIndex
    if (_cursorLine >= 0 && _change[_cursorLine].startIndex >= 0 && _change[_cursorLine].endIndex + 1 == _change[_cursorLine+1].startIndex) {
        while (_change.size() > _cursorLine+1) {
            _change.removeLast();
        }
    }
    // delay only if the change size is one line and it doesn't span multiple lines
    _delayable = _delayable && _change.size() == 1 && _change[0].startIndex == _change[0].endIndex;
    qDebug() << "set delayable to" << _delayable;
    _change.setDelayable(_delayable);
    return _change;
}

void HtmlBufferChangeParser::parseCharacter(const QChar &ch, int charCount)
{
    if (!_startParsing)
        return;
    if (ch == '\n' || ch == '\r') {
//        qDebug() << "encountered newline, charCount:" << charCount;
        // try to clean up lines after the cursorLine (including the current line)
        if (_cursorLine >= 0 && _change.last().startIndex >= 0) { // make sure we have at least one context line after the cursor line
            _stopParsing = true;
            return;
        }
        // set the cursor line after doing the check above forces the parser to at least
        // one more indexed line after the cursorline
        if (_cursorLine < 0 && charCount > _cursorPosition) {
            _cursorLine = _change.size() - 1;
        }
        // we always append new lines (let the <q> tag clear unuseful lines out!)
        _change.append(ChangedBufferLine());
    } else if (!ch.isNull()) {
        _change.last().line << ch;
    }
    if (charCount == _cursorPosition) {
        qDebug() << "reached cursor, current ch:" << ch << "charCount:" << charCount;
        // delay highlight if
        // 1. the character is a letter or number
        // 2. last time we didn't 'delay' highlighting - meaning there is no prediction currently
        //    we need to delay highlighting if we know a given character IS going to bring up prediction
        //    - in this case, we assume all white space characters can
        //    TODO: devise a strategy to reliably tell if there is prediction prompt
        _delayable =  ch.isLetterOrNumber() || (ch.isSpace() && !_delayable);
    }
}

void HtmlBufferChangeParser::parseTag(const QString &name, const QString &attributeName, const QString &attributeValue)
{
    if (name == "pre") {
        if (_cursorLine < 0) {
            // after seeing pre we can throw away all the old stuff
            _startParsing = false;
            _change = BufferStateChange();
        }
    } else if (name == "q") {
        Q_ASSERT(attributeName == "id" && !attributeValue.isEmpty());
        _startParsing = true;
        // after seeing q we can throw away all the previous lines
        // (when we haven't reached cursor yet)
        if (_cursorLine < 0) {
            while (_change.size() > 1)
                _change.removeFirst();
        }
        int parsedIndex = attributeValue.toInt();
        if (_change.last().startIndex < 0)
            _change.last().startIndex = parsedIndex;
        _change.last().endIndex = parsedIndex;
    }
}
void HtmlBufferChangeParser::parseHtmlCharacter(const QChar &ch) {}
bool HtmlBufferChangeParser::stopParsing() { return _stopParsing; }
void HtmlBufferChangeParser::reachedEnd() {}

QDebug operator<<(QDebug dbg, const ChangedBufferLine &line) {
    return dbg.nospace() << "ChangedBufferLine(startIndex:" << line.startIndex << ", "
            << "endIndex:" << line.endIndex << ", " << line.line << ")";
}
