/*
 * HtmlPlainTextExtractor.h
 *
 *  Created on: Jan 14, 2015
 *      Author: lingnan
 */

#ifndef HTMLPLAINTEXTEXTRACTOR_H_
#define HTMLPLAINTEXTEXTRACTOR_H_

#include <src/HtmlParser.h>
#include <src/Type.h>

class HtmlPlainTextExtractor : public HtmlParser
{
    Q_OBJECT
public:
    Q_SLOT QString extractPlainText(const QString &html);
    // the replacePlainText function assumes that the passed text is already plainText - this can speed things up
    Q_SLOT QString replacePlainText(const QString &plainText, const QList<QPair<TextSelection, QString> > &replaces);
    Q_SLOT bool hasPlainText(const QString &html);
private:
    enum PlainTextMode { Extract, Validate, Replace };
    QList<QPair<TextSelection, QString> > _replaces;
    PlainTextMode _mode;
    QString _buffer;
    bool _hasPlainText;
    bool stopParsing();
    void parseCharacter(const QChar &ch, int charCount);
    void parseTag(const QString &name, const QString &attributeName, const QString &attributeValue);
    void parseHtmlCharacter(const QChar &ch);
    void reachedEnd();
};

#endif /* HTMLPLAINTEXTEXTRACTOR_H_ */
