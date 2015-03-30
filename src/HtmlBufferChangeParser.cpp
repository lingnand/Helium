#include <src/HtmlBufferChangeParser.h>

HtmlBufferChangeParser::HtmlBufferChangeParser(): _lastDelayedLine(-1)
{
}

BufferStateChange HtmlBufferChangeParser::parseBufferChange(QTextStream &input, int cursorPosition)
{
    _startParsing = true;
    _stopParsing = false;
    _afterTTTag = false;
    _reachedCursor = false;
    _cursorPosition = cursorPosition;
    _change = BufferStateChange();
    parse(input);
    _change.setDelayable(_lastDelayedLine >= 0);
    return _change;
}

void HtmlBufferChangeParser::parseCharacter(const QChar &ch, int charCount)
{
    if (!_startParsing)
        return;
    if (ch == '\n') {
        if (_change.size() > 1 || // already recorded some change in the past
            // start recording change
            (!_afterTTTag && _change.last().line.size() > 0)) {
            // this line IS a change that should be recorded
            _change.append(ChangedBufferLine());
        }
        if (_reachedCursor)
            // stop parsing after the line which contains the cursor
            // we don't reset the cursor line
            _stopParsing = true;
        else
            _change.last() = ChangedBufferLine();
    } else if (!ch.isNull()) {
        _change.last().line << ch;
    }
    if (!_reachedCursor && charCount == _cursorPosition) {
        _reachedCursor = true;
        printf("reached cursor, current ch: %s, charCount: %d\n", qPrintable(QString(ch)), charCount);
        // if there is only one line of change
        if (_change.size() == 1 &&
                // not highlight if the character is a letter or number
                (ch.isLetterOrNumber()
                // or the last delayed line has been cleared
                // that means there is no prediction
                // we need to stop highlighting if we know the given character
                // IS going to bring up predictions
                // in this case, we assume all white space characters can
                // TODO: devise a strategy to reliably tell if there is prediction prompt
                // instead of just crudely assume ANYTHING will prompt for prediction
                ||  ((ch.isSpace() || ch == '\n') && _lastDelayedLine < 0))) {
            printf("entered delayed line for ch %s, _lastHighlightDelayedLine: %d\n", qPrintable(QString(ch)), _lastDelayedLine);
            _lastDelayedLine = _change.last().index;
        } else {
            _lastDelayedLine = -1;
        }
    }
    _afterTTTag = false;
}

void HtmlBufferChangeParser::parseTag(const QString &name, const QString &attributeName, const QString &attributeValue)
{
    if (name == "pre") {
        // after seeing pre we can throw away all the old stuff
        _startParsing = false;
        _change = BufferStateChange();
    } else if (name == "q") {
        Q_ASSERT(attributeName == "id" && !attributeValue.isEmpty());
        _startParsing = true;
        _change.last().index = attributeValue.toInt();
    } else if (name == "/q") {
        _afterTTTag = true;
    }

}
void HtmlBufferChangeParser::parseHtmlCharacter(const QChar &ch) {}
bool HtmlBufferChangeParser::stopParsing() { return _stopParsing; }
void HtmlBufferChangeParser::reachedEnd() {}
