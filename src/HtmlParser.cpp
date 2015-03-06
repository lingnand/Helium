/*
 * HtmlParser.cpp
 *
 *  Created on: Jan 14, 2015
 *      Author: lingnan
 */

#include <src/HtmlParser.h>
#include <stdio.h>

enum HtmlState { InChar, InTag };
enum CharState { Normal, InSpecialChar };
enum TagState { InPrefix, InAttributeName, InAttributeValue };

void HtmlParser::parse(QTextStream& input)
{
    // process the content
    // first find the regions to rehighlight
    HtmlState state = InChar;
    CharState charState = Normal;
    TagState tagState = InPrefix;
    int charCount = 0;
    QString specialCharStr, tagName, tagAttributeName, tagAttributeValue;
    // ch is the character just before the cursor
    QChar ch = BOF;
    while (true) {
        switch (state) {
            case InChar: {
                switch (charState) {
                    case Normal:
                        if (ch == BOF) {
                            parseCharacter(ch, charCount);
                        } else if (ch == '&') {
                            charState = InSpecialChar;
                        } else if (ch == '<') {
                            state = InTag;
                        } else {
                            charCount++;
                            parseCharacter(ch, charCount);
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
                                printf("unrecognized special char: %s\n", qPrintable(specialCharStr));
                            }
                            charCount++;
                            parseCharacter(ch, charCount);
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
        input >> ch;
        if (ch.isNull()) {
            // reached end of the content
            reachedEnd();
            break;
        }
        parseHtmlCharacter(ch);
    }
}

