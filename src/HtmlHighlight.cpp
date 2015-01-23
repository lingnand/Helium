/*
 * HtmlHighlight.cpp
 *
 *  Created on: Jan 14, 2015
 *      Author: lingnan
 */

#include <src/HtmlHighlight.h>
#include <stdio.h>

HtmlHighlight::HtmlHighlight(const QString &style):
    _sourceHighlight(std::string(style.toUtf8().constData()), "xhtml.outlang"),
    _lineCounter(0)
{
}

// filetype should be an empty string or a the name of a language file
// i.e. filetype.lang can be found in the datadir
void HtmlHighlight::setFiletype(const QString &filetype)
{
    if (filetype != _filetype) {
        _filetype = filetype;
        printf("Setting filetype to: %s\n", qPrintable(_filetype));
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
        emit filetypeChanged(_filetype);
    }
}


QString HtmlHighlight::highlightHtml(const QString &html, int cursorPosition, bool enableDelay)
{
    QString toParse = html;
    // strips the pre tags
    if (toParse.startsWith("<pre>"))
        toParse.remove(0, 5);
    if (toParse.endsWith("</pre>"))
        toParse.chop(6);
//    printf("parsing %s\n", qPrintable(toParse));
    // reset the variables
    _highlighted = false;
    _toHighlight = NoHighlight;
    _stopParsing = false;
    _afterTTTag = false;
    _reachedCursor = false;
    _currentHighlightStateData = _mainStateData;
    _toHighlightBuffer.clear();
    _htmlBuffer.clear();
    _currentLine = QStringPtr(new QString);
    _enableDelay = enableDelay;
    _cursorPosition = cursorPosition;
    if (!enableDelay)
        _lastHighlightDelayedLine.reset();
    _buffer.clear();
    int i = parse(toParse);
    if (_highlighted) {
        _buffer += _htmlBuffer;
        _buffer += toParse.right(toParse.length() - i);
        _buffer.prepend("<pre>");
        _buffer.append("</pre>");
        return _buffer;
    }
    return html;
}

bool HtmlHighlight::stopParsing()
{
    return _stopParsing;
}

void HtmlHighlight::parseCharacter(const QChar &ch, int charCount)
{
    if (!_reachedCursor && charCount == _cursorPosition) {
        _reachedCursor = true;
        printf("reached cursor, current ch: %s, current toHighlight: %s, current charCount, %d\n",
                qPrintable(QString(ch)), qPrintable(_toHighlightBuffer), charCount);
        if (!_toHighlight || _toHighlight == HighlightDelayed) {
            // if there is no highlight, or if it is to highlight the delay line and the current line IS the delay line AGAIN
            if (_enableDelay &&
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
                _lastHighlightDelayedLine = _currentLine;
                _stopParsing = true;
                return;
            } else {
                _toHighlight = HighlightCurrent;
            }
        }
    }
    if (ch == '\n') {
        if (!_afterTTTag  && !_toHighlightBuffer.isEmpty())
            _toHighlight = HighlightCurrent;
        if (highlightLine()) {
            _buffer += '\n';
        }
    } else if (!ch.isNull()) {
        _toHighlightBuffer += ch;
    }
    _afterTTTag = false;
}

bool HtmlHighlight::highlightLine()
{
    // if it's empty we are immediately done
    bool highlightedLine = _toHighlightBuffer.isEmpty();
    if (!highlightedLine) {
        if (_currentLine->isEmpty()) {
            // when the current line is empty and a flush character is met,
            // we SHOULD have toHighlight already set
            printf("empty current line detected with buffer %s; toHighlight: %d\n",
                    qPrintable(_toHighlightBuffer), _toHighlight);
            *_currentLine = QString::number(_lineCounter++);
            _toHighlight = HighlightCurrent;
        }
        if (_toHighlight) {
            Q_ASSERT(_currentHighlightStateData);
            HighlightLineStates &states = _highlightStateDataHash[*_currentLine];
            states.first = _currentHighlightStateData;
            _currentHighlightStateData = HighlightStateDataPtr(new HighlightStateData(*_currentHighlightStateData));
            _sourceHighlight.getHighlighter()->setCurrentState(_currentHighlightStateData->currentState);
            _sourceHighlight.getHighlighter()->setStateStack(_currentHighlightStateData->stateStack);
            _sourceHighlight.clearBuffer();
            _sourceHighlight.getHighlighter()->highlightParagraph(std::string(_toHighlightBuffer.toUtf8().constData()));
            QString highlightResult = QString::fromUtf8(_sourceHighlight.getBuffer().str().c_str());
            QString prefix = QString("<tt name='%1'>").arg(*_currentLine);
            QString postfix = "</tt>";
//                    printf("## Original html: %s\n", qPrintable(htmlBuffer));
            printf("## Highlight buffer: %s\n", qPrintable(_toHighlightBuffer));
            printf("## highlight result: %s\n", qPrintable(highlightResult));
            if (_filetype.isEmpty() || highlightResult != _toHighlightBuffer) {
                // if there is no change on the content to highlight
                // then we think there hasn't been any highlighting
                // NOTE: when the filetype is empty, what we are devote to doing is stripping format, thus
                // only in this case the highlight is set to true
                _highlighted = true;
            }
            _buffer += prefix;
            _buffer += highlightResult;
            _buffer += postfix;
            _currentHighlightStateData->currentState = _sourceHighlight.getHighlighter()->getCurrentState();
            states.second = _currentHighlightStateData;
            // reset the lastHighlightDelayedLine if necessary
            if (_currentLine == _lastHighlightDelayedLine) {
                printf("resetting lastHighlightDelayedLine\n");
                _lastHighlightDelayedLine.reset();
            }
            highlightedLine = true;
        } else {
            _buffer += _htmlBuffer;
            // if no need to highlight
            // then we should pass through to the end state of the current line if possible
            Q_ASSERT(_highlightStateDataHash.contains(*_currentLine));
            _currentHighlightStateData = _highlightStateDataHash[*_currentLine].second;
        }
        _toHighlightBuffer.clear();
    }
    _htmlBuffer.clear();
    // reset current line
    // here we set it to a new string so that _lastHighlightDelayedLine
    // will still point to the right line value
    _currentLine = QStringPtr(new QString);
    return highlightedLine;
}

void HtmlHighlight::parseTag(const QString &name, const QString &attributeName, const QString &attributeValue)
{
    if (name == "tt" && attributeName == "name") {
        printf("parsed tt tag with name %s\n", qPrintable(attributeValue));
        Q_ASSERT(!attributeValue.isEmpty());
        *_currentLine = attributeValue;
        if (_lastHighlightDelayedLine && *_currentLine == *_lastHighlightDelayedLine) {
            printf("Line %s detected to be last unhighlighted line!\n", qPrintable(*_currentLine));
            if (!_toHighlight)
                _toHighlight = HighlightDelayed;
            _lastHighlightDelayedLine = _currentLine;
        } else if (_toHighlight && _toHighlightBuffer.isEmpty() && _highlightStateDataHash.contains(*_currentLine)) {
            // in a force refresh procedure all the state hash is reset so we need to check for 'contains'
            // if the highlight buffer is not empty, that means it has not been flushed
            // we have highlighted some stuff in the past
            if (*_currentHighlightStateData == *_highlightStateDataHash[*_currentLine].first) {
                // setting noHighlight to false means we expect somewhere down to still be highlighted
                if (_lastHighlightDelayedLine)
                    _toHighlight = NoHighlight;
                else if (_reachedCursor)
                    // else we know that there is no need to continue
                    // note: we must guarantee that we've visited the cursor
                    _stopParsing = true;
            }
            printf("Line %s requested for highlight - stop parsing: %d\n",
                    qPrintable(*_currentLine), _stopParsing);
            // since we have highlighted stuff in the past we will
            // keep using the current state
        }
    } else if (name == "/tt") {
        // the close tag
        _afterTTTag = true;
    }
}

void HtmlHighlight::parseHtmlCharacter(const QChar &ch)
{
    _htmlBuffer += ch;
}

void HtmlHighlight::reachedEnd()
{
    highlightLine();
}
