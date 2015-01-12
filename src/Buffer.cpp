/*
 * Buffer.cpp
 *
 *  Created on: Nov 23, 2014
 *      Author: lingnan
 */

#include <src/Buffer.h>
#include <stdio.h>
#include <QSet>
#include <src/srchilite/sourcehighlight.h>
#include <src/srchilite/instances.h>
#include <src/srchilite/langmap.h>

/**
 * Utility class to deal with current highlighting state (and stack of states)
 */
struct HighlightStateData {
    /// the current state for the SourceHighlighter object
    srchilite::HighlightStatePtr currentState;

    /// the current stack for the SourceHighlighter object
    srchilite::HighlightStateStackPtr stateStack;

    HighlightStateData() {}
    /**
     * Performs a deep copy of the passed object (by duplicating the stack)
     * @param data
     */
    HighlightStateData(const HighlightStateData &data) :
                currentState(data.currentState),
                stateStack(srchilite::HighlightStateStackPtr(
                        new srchilite::HighlightStateStack(*(data.stateStack)))) {
    }

    HighlightStateData(srchilite::HighlightStatePtr currentState_,
            srchilite::HighlightStateStackPtr stateStack_) :
        currentState(currentState_), stateStack(stateStack_) {
    }

    bool operator==(const HighlightStateData &rhs) {
        return *currentState == *rhs.currentState && *stateStack == *rhs.stateStack;
    }
};

// for html parser
enum HtmlState { InChar, InTag };
enum CharState { Normal, AfterTTTag, InSpecialChar };
enum TagState { InPrefix, InAttributeName, InAttributeValue };
enum ToHighlightState { NoHighlight = 0, HighlightCurrent = 1, HighlightDelayed = 2};

# define BOF '\0'
/*
 * Buffer is responsible for continuously parsing the input and
 * rendering the html input for it
 *
 * TODO: do we need to shift the parsing into a worker thread?
 *
 */

Buffer::Buffer() :
    _sourceHighlight("default.style", "xhtml.outlang"),
    _langMap(srchilite::Instances::getLangMap()),
    _parsingContent(false), _lineCounter(0)
{
}

void Buffer::setName(const QString& name)
{
    if (name != _name) {
        _name = name;
        // try to set the filetype
        std::string ft = name.toUtf8().constData();
        ft = _langMap->getMappedFileNameFromFileName(ft);
        setFiletype(QString::fromUtf8(ft.c_str()));
        emit nameChanged(name);
    }
}

// filetype should be an empty string or a the name of a language file
// i.e. filetype.lang can be found in the datadir
void Buffer::setFiletype(const QString &filetype)
{
    if (filetype != _filetype) {
        _filetype = filetype;
        printf("Setting filetype of file %s to: %s\n", qPrintable(_name), qPrintable(_filetype));
        // clear the state cache
        _highlightStateDataHash.clear();
        if (_filetype.isEmpty()) {
            _sourceHighlight.setInputLang("default.lang");
        } else {
            _sourceHighlight.setInputLang(std::string(_filetype.toUtf8().constData()) + ".lang");
        }
        // initialize the main state data
        _mainStateData = HighlightStateDataPtr(new HighlightStateData(
                _sourceHighlight.getHighlighter()->getMainState(),
                srchilite::HighlightStateStackPtr(new srchilite::HighlightStateStack())
        ));
        // rehighlight the whole content
        parseContent(_content, 0, false, false);
        emit filetypeChanged(_filetype);
    }
}

void Buffer::setContent(const QString &content, int cursorPosition, bool enableDelay)
{
    parseContent(content, cursorPosition, _filetype.isEmpty(), true);
}

void Buffer::parseContent(const QString content, int cursorPosition, bool noHighlight, bool enableDelay)
{
    if (_parsingContent)
        return;
    _parsingContent = true;
    if (!noHighlight && tryHighlightContent(content, cursorPosition, enableDelay)) {
        // successfully highlighted content
        emit contentChanged(_content);
    } else if (content != _content) {
        // if highlight is not necessary
        _content = content;
        emit contentChanged(_content);
    }
    _parsingContent = false;
}

// TODO:solve the problem involving empty content lines' <tt></tt> got stripped
// try to highlight the given content using _content, return true if did highlight stuff, false otherwise
bool Buffer::tryHighlightContent(QString content, int cursorPosition, bool enableDelay)
{
    // strips the pre tags
    if (content.startsWith("<pre>"))
        content.remove(0, 5);
    if (content.endsWith("</pre>"))
        content.chop(6);
    printf("\n# parsing content with content: %s, cursorPosition: %d\n", qPrintable(content), cursorPosition);
    // process the content
    // first find the regions to rehighlight
    HtmlState state = InChar;
    CharState charState = Normal;
    TagState tagState = InPrefix;
    int charCount = 0;
    ToHighlightState toHighlight = NoHighlight;
    bool reachedCursor = false;
    bool highlighted = false;
    bool continueParsing = true;
    HighlightStateDataPtr currentHighlightStateData = _mainStateData;
    QString specialChar, tagName, tagAttributeName, tagAttributeValue,
            toHighlightBuffer, htmlBuffer;
    QStringPtr currentLine = QStringPtr(new QString);
    if (!enableDelay)
        _lastHighlightDelayedLine.reset();
    _content.clear();
    // i is the NEEDLE/CURSOR position inside the content
    int i = 0;
    // ch is the character just before the cursor
    QChar ch = BOF;
    // the condition for continue parsing
    // 1. toHighlight not set: that means the place to be highlighted hasn't been located
    //    (or the program expects another place somewhere down to be highlighted)
    // 2. if toHighlight is set: if continueParsing has been set to false, that means there is no more need
    //    to highlight more, thus we can stop
    while (!toHighlight || continueParsing) {
        switch (state) {
            case InChar:
                switch (charState) {
                    case Normal: case AfterTTTag:
                        if (ch == BOF) {
                            // do nothing if it's the starting character
                        } else if (ch == '&') {
                            charState = InSpecialChar;
                        } else if (ch == '<') {
                            state = InTag;
                        } else {
                            charCount++;
                            if (ch != '\n') {
                                // we don't add newline into the highlight buffer
                                toHighlightBuffer += ch;
                            }
                        }
                        break;
                    case InSpecialChar:
                        if (ch == ';') {
                            charCount++;
                            if (specialChar == "amp") {
                                toHighlightBuffer += '&';
                            } else if (specialChar == "lt") {
                                toHighlightBuffer += '<';
                            } else if (specialChar == "gt") {
                                toHighlightBuffer += '>';
                            }
                            specialChar.clear();
                            charState = Normal;
                        } else {
                            specialChar += ch;
                        }
                        break;
                }
                if (!reachedCursor && charCount == cursorPosition) {
                    reachedCursor = true;
                    printf("reached cursor, current ch: %s, current toHighlight: %s, current i: %d, current charCount, %d\n", qPrintable(QString(ch)), qPrintable(toHighlightBuffer), i, charCount);
                    if (!toHighlight || toHighlight == HighlightDelayed) {
                        // if there is no highlight, or if it is to highlight the delay line and the current line IS the delay line AGAIN
                        if (enableDelay &&
                                // not highlight if the character is a letter or number
                                (ch.isLetterOrNumber()
                                // or the last delayed line has been cleared
                                // that means there is no prediction
                                // we need to stop highlighting if we know the given character
                                // IS going to bring up predictions
                                // in this case, we assume all white space characters can
                                // TODO: devise a strategy to reliably tell if there is prediction prompt
                                // instead of just crudely assume ANYTHING will prompt for prediction
                                ||  ((ch.isSpace() || ch == '\n') && !_lastHighlightDelayedLine))) {
                            printf("entered delayed line for ch %s, _lastHighlightDelayedLine: %s\n",
                                    qPrintable(QString(ch)), _lastHighlightDelayedLine ? qPrintable(*_lastHighlightDelayedLine) : "NULL");
                            _lastHighlightDelayedLine = currentLine;
                            return false;
                        } else {
                            toHighlight = HighlightCurrent;
                        }
                    }
                } else if (ch == '\n' && charState != AfterTTTag && !toHighlightBuffer.isEmpty()) {
                    toHighlight = HighlightCurrent;
                }
                if (charState == AfterTTTag)
                    charState = Normal;
                break;
            case InTag:
                if (ch == '>') {
                    // reached tag end
                    if (tagName == "tt" && tagAttributeName == "name") {
                        printf("parsed tt tag with name %s\n", qPrintable(tagAttributeValue));
                        Q_ASSERT(!tagAttributeValue.isEmpty());
                        *currentLine = tagAttributeValue;
                        if (_lastHighlightDelayedLine && *currentLine == *_lastHighlightDelayedLine) {
                            printf("Line %s detected to be last unhighlighted line!\n", qPrintable(*currentLine));
                            if (!toHighlight)
                                toHighlight = HighlightDelayed;
                            _lastHighlightDelayedLine = currentLine;
                        } else if (toHighlight && toHighlightBuffer.isEmpty() && _highlightStateDataHash.contains(*currentLine)) {
                            // in a force refresh procedure all the state hash is reset do we need to check for 'contains'
                            // if the highlight buffer is not empty, that means it has not been flushed
                            // we have highlighted some stuff in the past
                            Q_ASSERT(currentHighlightStateData);
                            if (*currentHighlightStateData == *_highlightStateDataHash[*currentLine].first) {
                                // setting noHighlight to false means we expect somewhere down to still be highlighted
                                if (_lastHighlightDelayedLine)
                                    toHighlight = NoHighlight;
                                else if (reachedCursor)
                                    // else we know that there is no need to continue
                                    // note: we must guarantee that we've visited the cursor
                                    continueParsing = false;
                            }
                            printf("Line %s requested for highlight - continue parsing: %d\n",
                                    qPrintable(*currentLine), continueParsing);
                            // since we have highlighted stuff in the past we will
                            // keep using the current state
                        }
                    } else if (tagName == "/tt") {
                        // the close tag
                        charState = AfterTTTag;
                    }
                    tagName.clear();
                    tagAttributeName.clear();
                    tagAttributeValue.clear();
                    state = InChar;
                    tagState = InPrefix;
                } else {
                    switch (tagState) {
                        case InPrefix:
                            if (ch == ' ' && tagName == "tt") {
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
                            if (ch != ' ' && ch != '\'') {
                                tagAttributeValue += ch;
                            }
                            break;
                    }
                }
                break;
        }
        if (ch == '\n' || i == content.length()) {
            // we met the end of the line or the last character
            // we've encountered a new line
            // the default action is to highlight the stuff in the current buffer
            // reset the last accumulated tohighlight if haven't reached cursor
            if (toHighlightBuffer.isEmpty()) {
                if (ch == '\n')
                    _content += '\n';
            } else {
                if (currentLine->isEmpty()) {
                    // when the current line is empty and a flush character is met,
                    // we SHOULD have toHighlight already set
                    printf("empty current line detected with buffer %s; toHighlight: %d\n",
                            qPrintable(toHighlightBuffer), toHighlight);
                    *currentLine = QString::number(_lineCounter++);
                    toHighlight = HighlightCurrent;
                }
                if (toHighlight) {
                    Q_ASSERT(currentHighlightStateData);
                    HighlightLineStates &states = _highlightStateDataHash[*currentLine];
                    states.first = currentHighlightStateData;
                    currentHighlightStateData = HighlightStateDataPtr(new HighlightStateData(*currentHighlightStateData));
                    _sourceHighlight.getHighlighter()->setCurrentState(currentHighlightStateData->currentState);
                    _sourceHighlight.getHighlighter()->setStateStack(currentHighlightStateData->stateStack);
                    _sourceHighlight.clearBuffer();
                    _sourceHighlight.getHighlighter()->highlightParagraph(std::string(toHighlightBuffer.toUtf8().constData()));
                    QString highlightResult = QString::fromUtf8(_sourceHighlight.getBuffer().str().c_str());
                    QString prefix = QString("<tt name='%1'>").arg(*currentLine);
                    QString postfix = "</tt>";
//                    printf("## Original html: %s\n", qPrintable(htmlBuffer));
                    printf("## Highlight buffer: %s\n", qPrintable(toHighlightBuffer));
                    printf("## highlight result: %s\n", qPrintable(highlightResult));
                    if (_filetype.isEmpty() || highlightResult != toHighlightBuffer) {
                        // if there is no change on the content to highlight
                        // then we think there hasn't been any highlighting
                        // NOTE: when the filetype is empty, what we are devote to doing is stripping format, thus
                        // only in this case the highlight is set to true
                        highlighted = true;
                    }
                    _content += prefix;
                    _content += highlightResult;
                    _content += postfix;
                    if (ch == '\n')
                        _content += '\n';
                    currentHighlightStateData->currentState = _sourceHighlight.getHighlighter()->getCurrentState();
                    states.second = currentHighlightStateData;
                    // reset the lastHighlightDelayedLine if necessary
                    if (currentLine == _lastHighlightDelayedLine) {
                        printf("resetting lastHighlightDelayedLine\n");
                        _lastHighlightDelayedLine.reset();
                    }
                } else {
                    _content += htmlBuffer;
                    // if no need to highlight
                    // then we should pass through to the end state of the current line if possible
                    Q_ASSERT(_highlightStateDataHash.contains(*currentLine));
                    currentHighlightStateData = _highlightStateDataHash[*currentLine].second;
                }
                toHighlightBuffer.clear();
            }
            htmlBuffer.clear();
            // reset current line
            // here we set it to a new string so that _lastHighlightDelayedLine
            // will still point to the right line value
            currentLine = QStringPtr(new QString);
        }
        if (i == content.length()) {
            // reached end of the content
            break;
        } else {
            // increment the cursor
            ch = content[i++];
            htmlBuffer += ch;
        }
    }
    if (highlighted) {
        // after the parser finishes, we need to take whatever is left inside the content and append
        // that to the html we just obtained
//        printf("htmlBuffer: %s\n", qPrintable(htmlBuffer));
        _content += htmlBuffer;
//        printf("leftover: %s\n", qPrintable(content.right(content.length() - i)));
        _content += content.right(content.length() - i);
        // add the <pre> and </pre>
        if (!_content.startsWith("<pre>"))
            _content.prepend("<pre>");
        if (!_content.endsWith("</pre>"))
            _content.append("</pre>");
        return true;
    }
    return false;
}
