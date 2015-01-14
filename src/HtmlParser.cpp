/*
 * HtmlParser.cpp
 *
 *  Created on: Jan 14, 2015
 *      Author: lingnan
 */

#include <src/HtmlParser.h>

enum HtmlState { InChar, InTag };
enum CharState { Normal, InSpecialChar };
enum TagState { InPrefix, InAttributeName, InAttributeValue };

unsigned int HtmlParser::parse(const QString& html)
{
    // process the content
    // first find the regions to rehighlight
    HtmlState state = InChar;
    CharState charState = Normal;
    TagState tagState = InPrefix;
    unsigned int charCount = 0;
    bool _reachedCursor = false;
    QString specialChar, tagName, tagAttributeName, tagAttributeValue;
    // i is the NEEDLE/CURSOR position inside the content
    unsigned int i = 0;
    // ch is the character just before the cursor
    QChar ch;
    while (true) {
        switch (state) {
            case InChar: {
                QChar parsed;
                switch (charState) {
                    case Normal:
                        if (ch == '&') {
                            charState = InSpecialChar;
                        } else if (ch == '<') {
                            state = InTag;
                        } else {
                            parsed = ch;
                        }
                        break;
                    case InSpecialChar:
                        if (ch == ';') {
                            charCount++;
                            if (specialChar == "amp") {
                                parsed = '&';
                            } else if (specialChar == "lt") {
                                parsed = '<';
                            } else if (specialChar == "gt") {
                                parsed = '>';
                            }
                            specialChar.clear();
                            charState = Normal;
                        } else {
                            specialChar += ch;
                        }
                        break;
                }
                if (!parsed.isNull()) {
                    charCount++;
                    parseCharacter(parsed, charCount);
                }
                break;
            }
            case InTag:
                if (ch == '>') {
                    parseTag(tagName, tagAttributeName, tagAttributeValue);
                    tagName.clear();
                    tagAttributeName.clear();
                    tagAttributeValue.clear();
                    state = InChar;
                    tagState = InPrefix;
                } else {
                    switch (tagState) {
                        case InPrefix:
                            if (ch == ' ') {
                                tagState = InAttributeName;
                            } else {
                                tagName += ch;
                            }
                            break;
                        case InAttributeName:
                            if (ch != ' ') {
                                if (ch == '=') {
                                    tagState = InAttributeValue;
                                } else {
                                    tagAttributeName += ch;
                                }
                            }
                            break;
                        case InAttributeValue:
                            // now we only support one attribute
                            if (ch != ' ' && ch != '\'') {
                                tagAttributeValue += ch;
                            }
                            break;
                    }
                }
                break;
        }
        if (stopParsing()) {
            break;
        } if (i == html.length()) {
            // reached end of the content
            reachedEnd();
            break;
        } else {
            // increment the cursor
            ch = html[i++];
            parseHtmlCharacter(ch);
        }
    }
    return i;
}

