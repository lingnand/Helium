/*
 * HtmlParser.h
 *
 *  Created on: Jan 14, 2015
 *      Author: lingnan
 */

#ifndef HTMLPARSER_H_
#define HTMLPARSER_H_

#define BOF '\0'

#include <QTextStream>

struct ParserPosition {
    int charCount; // the character count at the given location in the plaintext
    int htmlCount; // the character count at the given location in the html
    ParserPosition(int _charCount=0, int _htmlCount=0):
        charCount(_charCount), htmlCount(_htmlCount) {}
};

/* A general purpose html parser
 * only works for the simple syntax inside a textarea
 * subclass should override the necessary functions
 */
class HtmlParser : public QObject
{
    Q_OBJECT
protected:
    void parse(const QString &input, ParserPosition position);
    virtual bool stopParsing() = 0;
    // parse character begins from BOF
    virtual void parseCharacter(const QChar &ch, int charCount) = 0;
    virtual void parseTag(const QString &name, const QString &attributeName, const QString &attributeValue) = 0;
    virtual void reachedEnd() = 0;
};

QDebug operator<<(QDebug dbg, const ParserPosition &line);

#endif /* HTMLPARSER_H_ */
