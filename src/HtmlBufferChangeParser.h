#ifndef HTMLBUFFERCHANGEPARSER_H_
#define HTMLBUFFERCHANGEPARSER_H_

#include <QTextStream>
#include <src/HtmlParser.h>
#include <src/BufferState.h>

class HtmlBufferChangeParser : public HtmlParser
{
public:
    struct ChangedBufferLine
    {
        // the presumed index for this line
        int index;
        BufferLine line;
        ChangedBufferLine(): index(-1), line(BufferLine()) {}
    }

    class BufferStateChange : QList<ChangedBufferLine>
    {
        friend class HtmlBufferChangeParser;
    public:
        // the index of the first change
        // NOTE: this should always be a valid index
        int startIndex() { return at(0).index; }
        bool delayable() { return _delayable; }
    private:
        // whether this buffer change is delayable
        _delayable;
    }

    BufferStateChange parseBufferChange(QTextStream &input);
private:
    BufferStateChange _change;
    bool stopParsing();
    void parseCharacter(const QChar &ch, int charCount);
    void parseTag(const QString &name, const QString &attributeName, const QString &attributeValue);
    void parseHtmlCharacter(const QChar &ch);
    void reachedEnd();
}

#endif /* HTMLBUFFERCHANGEPARSER_H_ */
