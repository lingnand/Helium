/*
 * HtmlParser.cpp
 *
 *  Created on: Jan 14, 2015
 *      Author: lingnan
 */

#include <HtmlParser.h>
#include <QDebug>

enum HtmlState { InChar, InTag };
enum CharState { Normal, InSpecialChar };
enum TagState { InPrefix, InAttributeName, InAttributeValue };

void HtmlParser::parse(const QString &input, ParserPosition position)
{
    // process the content
    // first find the regions to rehighlight
    HtmlState state = InChar;
    CharState charState = Normal;
    TagState tagState = InPrefix;
    QString specialCharStr, tagName, tagAttributeName, tagAttributeValue;
    // ch is the character just before the cursor
    QChar ch = position.htmlCount ? input[position.htmlCount-1] : BOF;
    while (true) {
        switch (state) {
            case InChar: {
                switch (charState) {
                    case Normal:
                        if (ch == BOF) {
                            parseCharacter(ch, position.charCount);
                        } else if (ch == '&') {
                            charState = InSpecialChar;
                        } else if (ch == '<') {
                            state = InTag;
                        } else {
                            position.charCount++;
                            parseCharacter(ch, position.charCount);
                        }
                        break;
                    case InSpecialChar:
                        if (ch == ';') {
                            if (specialCharStr == "amp") {
                                ch = '&';
                            } else if (specialCharStr == "lt") {
                                ch = '<';
                            } else if (specialCharStr == "gt") {
                                ch = '>';
                            } else {
                                // set it to null character at the moment
                                ch = '\0';
                                qCritical() << "unrecognized special char:" << specialCharStr;
                            }
                            position.charCount++;
                            parseCharacter(ch, position.charCount);
                            specialCharStr.clear();
                            charState = Normal;
                        } else {
                            specialCharStr += ch;
                        }
                        break;
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
        }
        if (position.htmlCount >= input.size()) {
            // reached end of the content
            reachedEnd();
            break;
        }
        ch = input[position.htmlCount++];
    }
}

QDebug operator<<(QDebug dbg, const ParserPosition &pos)
{
    dbg.nospace() << "ParserPosition(charCount:" << pos.charCount <<
            ", htmlCount:" << pos.htmlCount << ")";
    return dbg.space();
}
