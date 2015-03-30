#ifndef HTMLBUFFERCHANGEPARSER_H_
#define HTMLBUFFERCHANGEPARSER_H_

#include <QTextStream>
#include <src/HtmlParser.h>
#include <src/BufferState.h>

struct ChangedBufferLine
{
    // the presumed index for this line
    int index;
    BufferLine line;
    ChangedBufferLine(): index(-1), line(BufferLine()) {}
};

class BufferStateChange : public QList<ChangedBufferLine>
{
    friend class HtmlBufferChangeParser;
public:
    // the index of the first change
    // NOTE: this should always be a valid index
    int startIndex() { return at(0).index; }
    bool delayable() { return _delayable; }
    BufferStateChange(): _delayable(false) {
        // a BufferStateChange always contain at least one changedBufferLine
        append(ChangedBufferLine());
    }
private:
    // whether this buffer change is delayable
    bool _delayable;
    void setDelayable(bool delayable) { _delayable = delayable; }
};

class HtmlBufferChangeParser : public HtmlParser
{
public:
    HtmlBufferChangeParser();
    BufferStateChange parseBufferChange(QTextStream &input, int cursorPosition);
private:
    bool _startParsing;
    bool _stopParsing;
    bool _afterTTTag;
    bool _reachedCursor;
    int _lastDelayedLine;
    int _cursorPosition;
    BufferStateChange _change;
    void parseCharacter(const QChar &ch, int charCount);
    void parseTag(const QString &name, const QString &attributeName, const QString &attributeValue);
    void parseHtmlCharacter(const QChar &ch);
    bool stopParsing();
    void reachedEnd();
};

#endif /* HTMLBUFFERCHANGEPARSER_H_ */
