/*
 * HtmlParser.h
 *
 *  Created on: Jan 14, 2015
 *      Author: lingnan
 */

#ifndef HTMLPARSER_H_
#define HTMLPARSER_H_

#define BOF '\0'

#include <QObject>

/* A general purpose html parser
 * only works for the simple syntax inside a textarea
 * subclass should override the necessary functions
 */
class HtmlParser : public QObject
{
    Q_OBJECT
protected:
    // return the last cursor index
    int parse(const QString& html);
    virtual bool stopParsing() = 0;
    // parse character begins from BOF
    virtual void parseCharacter(const QChar &ch, int charCount) = 0;
    virtual void parseTag(const QString &name, const QString &attributeName, const QString &attributeValue) = 0;
    // this starts from the first character
    virtual void parseHtmlCharacter(const QChar &ch) = 0;
    virtual void reachedEnd() = 0;
};

#endif /* HTMLPARSER_H_ */
