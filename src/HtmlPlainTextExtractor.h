/*
 * HtmlPlainTextExtractor.h
 *
 *  Created on: Jan 14, 2015
 *      Author: lingnan
 */

#ifndef HTMLPLAINTEXTEXTRACTOR_H_
#define HTMLPLAINTEXTEXTRACTOR_H_

#include <src/HtmlParser.h>

class HtmlPlainTextExtractor : public HtmlParser
{
    Q_OBJECT
public:
    Q_SLOT QString extractPlainText(const QString &html) {
        _buffer.clear();
        parse(html);
        return _buffer;
    }
private:
    QString _buffer;
    bool stopParsing() { return false; }
    void parseCharacter(const QChar &ch, unsigned int charCount) { _buffer += ch; }
    void parseTag(const QString &name, const QString &attributeName, const QString &attributeValue) {}
    void parseHtmlCharacter(const QChar &ch) {}
    void reachedEnd() {}
};

#endif /* HTMLPLAINTEXTEXTRACTOR_H_ */
