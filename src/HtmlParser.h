/*
 * HtmlParser.h
 *
 *  Created on: Jan 14, 2015
 *      Author: lingnan
 */

#ifndef HTMLPARSER_H_
#define HTMLPARSER_H_

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
    unsigned int parse(const QString& html);
    virtual bool stopParsing() = 0;
    virtual void parseCharacter(const QChar &ch, unsigned int charCount) = 0;
    virtual void parseTag(const QString &name, const QString &attributeName, const QString &attributeValue) = 0;
    virtual void parseHtmlCharacter(const QChar &ch) = 0;
    virtual void reachedEnd() = 0;
};

#endif /* HTMLPARSER_H_ */
