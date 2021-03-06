#ifndef HTMLBUFFERCHANGEPARSER_H_
#define HTMLBUFFERCHANGEPARSER_H_

#include <QTextStream>
#include <HtmlParser.h>
#include <BufferState.h>
#include <QDebug>

struct ChangedBufferLine
{
    int index;
    BufferLine line;
    ChangedBufferLine(int i=-1): index(i), line(BufferLine()) {}
};

class BufferStateChange : public QList<ChangedBufferLine>
{
    friend class HtmlBufferChangeParser;
public:
    // the index of the first change
    int startIndex() const { return _startIndex; }
    bool delayable() const { return _delayable; }
    bool atEnd() const { return _atEnd; }
    BufferStateChange(): _startIndex(0), _delayable(true), _atEnd(false) { // assume delayable by default
        append(ChangedBufferLine());
    }
private:
    int _startIndex;
    // whether this buffer change is delayable
    bool _delayable;
    // whether this change concludes the entire buffer
    bool _atEnd;
};

class HtmlBufferChangeParser : public HtmlParser
{
public:
    HtmlBufferChangeParser();
    BufferStateChange parseBufferChange(const QString &input, ParserPosition position, int cursorPosition);
private:
    bool _stopParsing;
    bool _lastDelayable;
    int _cursorPosition;
    int _cursorLine; // index of cursorLine inside _change; or -1 when not found yet
    BufferStateChange _change;
    void parseCharacter(const QChar &ch, int charCount);
    void parseTag(const QString &name, const QString &attributeName, const QString &attributeValue);
    bool stopParsing();
    void reachedEnd();
};

QDebug operator<<(QDebug dbg, const ChangedBufferLine &line);
QDebug operator<<(QDebug dbg, const BufferStateChange &change);

#endif /* HTMLBUFFERCHANGEPARSER_H_ */
