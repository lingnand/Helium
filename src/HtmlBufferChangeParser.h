#ifndef HTMLBUFFERCHANGEPARSER_H_
#define HTMLBUFFERCHANGEPARSER_H_

#include <QTextStream>
#include <src/HtmlParser.h>
#include <src/BufferState.h>
#include <QDebug>

struct ChangedBufferLine
{
    int startIndex; // the presumed start index for this line
    int endIndex; // the presumed end index for this line
    BufferLine line;
    ChangedBufferLine(): startIndex(-1), endIndex(-1), line(BufferLine()) {}
};

class BufferStateChange : public QList<ChangedBufferLine>
{
    friend class HtmlBufferChangeParser;
public:
    // the index of the first change
    int startIndex() { return at(0).startIndex; }
    bool delayable() { return _delayable; }
    BufferStateChange(): _delayable(false) {
        // a BufferStateChange always contain at least one changedBufferLine
        append(ChangedBufferLine());
    }
private:
    // whether this buffer change is delayable
    bool _delayable;
};

class HtmlBufferChangeParser : public HtmlParser
{
public:
    HtmlBufferChangeParser();
    BufferStateChange parseBufferChange(QTextStream &input, int cursorPosition);
private:
    bool _startParsing;
    bool _stopParsing;
    bool _afterQTag;
    bool _lastDelayable;
    int _cursorPosition;
    int _cursorLine; // index of cursorLine inside _change; or -1 when not found yet
    BufferStateChange _change;
    void parseCharacter(const QChar &ch, int charCount);
    void parseTag(const QString &name, const QString &attributeName, const QString &attributeValue);
    void parseHtmlCharacter(const QChar &ch);
    bool stopParsing();
    void reachedEnd();
};

QDebug operator<<(QDebug dbg, const ChangedBufferLine &line);
QDebug operator<<(QDebug dbg, const BufferStateChange &change);

#endif /* HTMLBUFFERCHANGEPARSER_H_ */
