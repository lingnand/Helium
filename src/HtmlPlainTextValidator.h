/*
 * HtmlPlainTextValidator.h
 *
 *  Created on: Jan 18, 2015
 *      Author: lingnan
 */

#ifndef HTMLPLAINTEXTVALIDATOR_H_
#define HTMLPLAINTEXTVALIDATOR_H_

#include <src/HtmlParser.h>

class HtmlPlainTextValidator : public HtmlParser
{
    Q_OBJECT
public:
    Q_SLOT bool hasPlainText(const QString &html) {
        _hasPlainText = false;
        parse(html);
        return _hasPlainText;
    }
    static bool HtmlHasPlainText(const QString &html) {
        HtmlPlainTextValidator v;
        return v.hasPlainText(html);
    }
private:
    bool _hasPlainText;
    bool stopParsing() { return _hasPlainText; }
    void parseCharacter(const QChar &ch, int charCount) {
        if (!ch.isNull())
            _hasPlainText = true;
    }
    void parseTag(const QString &name, const QString &attributeName, const QString &attributeValue) {}
    void parseHtmlCharacter(const QChar &ch) {}
    void reachedEnd() {}
};

#endif /* HTMLPLAINTEXTVALIDATOR_H_ */
