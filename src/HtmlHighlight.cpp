/*
 * HtmlHighlight.cpp
 *
 *  Created on: Jan 14, 2015
 *      Author: lingnan
 */

#include <stdio.h>
#include <src/HtmlHighlight.h>
#include <src/BufferState.h>

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
        if (!_filetype.isEmpty()) {
            _sourceHighlight.setInputLang(std::string(_filetype.toUtf8().constData()) + ".lang");
            // initialize the main state data
            _mainStateData = HighlightStateDataPtr(new HighlightStateData(
                    _sourceHighlight.getHighlighter()->getMainState(),
                    srchilite::HighlightStateStackPtr(new srchilite::HighlightStateStack())
            ));
        }
        emit filetypeChanged(_filetype);
    }
}

// modify the bufferState to pull in the changes in the input
// if enableDelay is set to true, we do some magic to not modify the state in some cases
bool HtmlHighlight::highlightChange(BufferState &state, QTextStream &input, int cursorPosition, bool enableDelay)
{
    BufferStateChange change = _bufferChangeParser.parseBufferChange(input, cursorPosition);
    // pull in the changes
    int bufferIndex = qMax(change.startIndex(), 0);
    int changeIndex = 0;
    int offset = 0;
    HighlightStateDataPtr currentHighlightData;
    int beforeStartIndex = bufferIndex - 1;
    if (beforeStartIndex >= 0 && beforeStartIndex < state.size()) {
        currentHighlightData = state[beforeStartIndex].endHighlightState();
    } else {
        currentHighlightData = _mainStateData;
    }
    while (changeIndex < change.size()) {
        ChangedBufferLine &change = change[changeIndex];
        BufferLine &changedLine = change.line;
        currentHighlightData = HighlightStateDataPtr(new HighlightStateData(*currentHighlightData));
        _sourceHighlight.getHighlighter()->setCurrentState(currentHighlightData->currentState);
        _sourceHighlight.getHighlighter()->setStateStack(currentHighlightData->stateStack);
        _sourceHighlight.clearBuffer();
        QByteArray plainText;
        QTextStream stream(&plainText);
        changedLine.writePlainText(stream);
        stream << flush;
        _sourceHighlight.getHighlighter()->highlightParagraph(std::string(plainText.constData()));
        changedLine.setHighlightText(QString::fromUtf8(_sourceHighlight.getBuffer().str().c_str()));
//        if (_filetype.isEmpty() || highlightResult != _toHighlightBuffer) {
            // if there is no change on the content to highlight
            // then we think there hasn't been any highlighting
            // NOTE: when the filetype is empty, what we are devote to doing is stripping format, thus
            // only in this case the highlight is set to true
//            _highlighted = true;
//        }
        currentHighlightData->currentState = _sourceHighlight.getHighlighter()->getCurrentState();
        changedLine.setEndHighlightState(currentHighlightData);
        if (change.index < 0) {
            // we need to insert this new line
            state.insert(bufferIndex, changedLine);
            offset++;
        } else {
            // we need to delete the lines until the index match up
            while (bufferIndex != change.index+offset) {
                state.removeAt(bufferIndex);
                offset--;
            }
            state[bufferIndex] = changedLine;
        }
        changeIndex++;
        bufferIndex++;
    }
    // TODO: we still need to keep on highlighting until we meet something with the same start state
    return !enableDelay || !change.delayable();
}

bool HtmlHighlight::highlightHtml(QTextStream &input, QTextStream &output, int cursorPosition, bool enableDelay)
{
    _enableDelay = enableDelay;
    _cursorPosition = cursorPosition;
    _reachedCursor = false;
    if (!enableDelay)
        _lastHighlightDelayedLine.reset();

    return highlightHtmlBasic(input, output);
}


void HtmlHighlight::parseCharacter(const QChar &ch, int charCount)
{
    if (!_startParsing)
        return;
    if (!_reachedCursor && charCount == _cursorPosition) {
        _reachedCursor = true;
        printf("reached cursor, current ch: %s\n, charCount: %d\n", qPrintable(QString(ch)), charCount);
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
                    ||  ((ch.isSpace() || ch == '\n') && _lastHighlightDelayedLine < 0))) {
                printf("entered delayed line for ch %s, _lastHighlightDelayedLine: %d\n", qPrintable(QString(ch)), _lastHighlightDelayedLine);
                _lastHighlightDelayedLine = _currentLine;
                _stopParsing = true;
                return;
            } else {
                _toHighlight = HighlightCurrent;
            }
        }
    }
    // TODO: reached refactoring here!
    if (ch == '\n') {
        if (!_afterTTTag  && !_toHighlightBuffer.isEmpty())
            _toHighlight = HighlightCurrent;
        if (highlightLine()) {
            (*_output) << '\n';
        }
    } else if (!rch.isNull()) {
        _currentLine.plainText += rch;
    }
    _afterTTTag = false;
}

// put the highlighted content into the line
bool HtmlHighlight::highlightLine(BufferLine &line)
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
            (*_output) << prefix;
            (*_output) << highlightResult;
            (*_output) << postfix;
            _currentHighlightStateData->currentState = _sourceHighlight.getHighlighter()->getCurrentState();
            states.second = _currentHighlightStateData;
            // reset the lastHighlightDelayedLine if necessary
            if (_mode == Incremental && _currentLine == _lastHighlightDelayedLine) {
                printf("resetting lastHighlightDelayedLine\n");
                _lastHighlightDelayedLine.reset();
            }
            highlightedLine = true;
        } else {
            (*_output) << _htmlBuffer;
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
    if (name == "q") {
        Q_ASSERT(attributeName == "id" && !attributeValue.isEmpty());
        if (!_startParsing) {
            _startParsing = true;
        }
        _tempLine.index = attributeValue.toInt();
    } else if (name == "/q") {
        if (_reachedCursor) // we've reached cursor so we don't need parse anymore (not expecting any change later down the line)
            _stopParsing = true;
        else
            _afterTTTag = true;
    }
}

bool HtmlHighlight::stopParsing() { return _stopParsing; }
void HtmlHighlight::parseHtmlCharacter(const QChar &ch) {}
void HtmlHighlight::reachedEnd() {}

bool HtmlHighlight::replaceHtml(QTextStream &input, QTextStream &output, const QList<QPair<TextSelection, QString> > &replaces)
{
    if (replaces.count() == 0)
        return false;

    _replaces = replaces;

    return highlightHtmlBasic(input, output);
}

