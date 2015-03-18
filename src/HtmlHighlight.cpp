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
    _sourceHighlight(std::string(style.toUtf8().constData()), "xhtml.outlang")
{
}

const QString &HtmlHighlight::filetype() { return _filetype; }

// filetype should be an empty string or a the name of a language file
// i.e. filetype.lang can be found in the datadir
void HtmlHighlight::setFiletype(const QString &filetype)
{
    if (filetype != _filetype) {
        _filetype = filetype;
        printf("Setting filetype to: %s\n", qPrintable(_filetype));
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

HighlightStateDataPtr HtmlHighlight::highlightLine(BufferLine &line, HighlightStateDataPtr startState)
{
    if (filetype().isEmpty()) {
        QString output;
        QTextStream stream(&output);
        line.writePreText(stream);
        stream << flush;
        line.setHighlightText(output);
        line.endHighlightState().reset();
    } else {
        HighlightStateDataPtr state = HighlightStateDataPtr(new HighlightStateData(*startState));
        _sourceHighlight.getHighlighter()->setCurrentState(state->currentState);
        _sourceHighlight.getHighlighter()->setStateStack(state->stateStack);
        _sourceHighlight.clearBuffer();
        QByteArray plainText;
        QTextStream stream(&plainText);
        line.writePlainText(stream);
        stream << flush;
        _sourceHighlight.getHighlighter()->highlightParagraph(std::string(plainText.constData()));
        line.setHighlightText(QString::fromUtf8(_sourceHighlight.getBuffer().str().c_str()));
        state->currentState = _sourceHighlight.getHighlighter()->getCurrentState();
        line.setEndHighlightState(state);
    }
    return line.endHighlightState();
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
    HighlightStateDataPtr lastEndHighlightData = currentHighlightData;
    while (changeIndex < change.size()) {
        ChangedBufferLine &ch = change[changeIndex];
        currentHighlightData = highlightLine(ch.line, currentHighlightData);
        if (ch.index < 0) {
            // we need to insert this new line
            state.insert(bufferIndex, ch.line);
            offset++;
        } else {
            // we need to delete the lines until the index match up
            while (bufferIndex != ch.index+offset) {
                state.removeAt(bufferIndex);
                offset--;
            }
            lastEndHighlightData = state[bufferIndex].endHighlightState();
            state[bufferIndex] = ch.line;
        }
        changeIndex++;
        bufferIndex++;
    }
    while (bufferIndex < state.size() && *lastEndHighlightData != *currentHighlightData){
        lastEndHighlightData = state[bufferIndex].endHighlightState();
        currentHighlightData = highlightLine(state[bufferIndex], currentHighlightData);
        bufferIndex++;
    }
    return !enableDelay || !change.delayable();
}

// put the highlighted content into the line
//bool HtmlHighlight::highlightLine(BufferLine &line)
//{
//    // if it's empty we are immediately done
//    bool highlightedLine = _toHighlightBuffer.isEmpty();
//    if (!highlightedLine) {
//        if (_currentLine->isEmpty()) {
//            // when the current line is empty and a flush character is met,
//            // we SHOULD have toHighlight already set
//            printf("empty current line detected with buffer %s; toHighlight: %d\n",
//                    qPrintable(_toHighlightBuffer), _toHighlight);
//            *_currentLine = QString::number(_lineCounter++);
//            _toHighlight = HighlightCurrent;
//        }
//        if (_toHighlight) {
//            Q_ASSERT(_currentHighlightStateData);
//            HighlightLineStates &states = _highlightStateDataHash[*_currentLine];
//            states.first = _currentHighlightStateData;
//            _currentHighlightStateData = HighlightStateDataPtr(new HighlightStateData(*_currentHighlightStateData));
//            _sourceHighlight.getHighlighter()->setCurrentState(_currentHighlightStateData->currentState);
//            _sourceHighlight.getHighlighter()->setStateStack(_currentHighlightStateData->stateStack);
//            _sourceHighlight.clearBuffer();
//            _sourceHighlight.getHighlighter()->highlightParagraph(std::string(_toHighlightBuffer.toUtf8().constData()));
//            QString highlightResult = QString::fromUtf8(_sourceHighlight.getBuffer().str().c_str());
//            QString prefix = QString("<tt name='%1'>").arg(*_currentLine);
//            QString postfix = "</tt>";
////                    printf("## Original html: %s\n", qPrintable(htmlBuffer));
//            printf("## Highlight buffer: %s\n", qPrintable(_toHighlightBuffer));
//            printf("## highlight result: %s\n", qPrintable(highlightResult));
//            if (_filetype.isEmpty() || highlightResult != _toHighlightBuffer) {
//                // if there is no change on the content to highlight
//                // then we think there hasn't been any highlighting
//                // NOTE: when the filetype is empty, what we are devote to doing is stripping format, thus
//                // only in this case the highlight is set to true
//                _highlighted = true;
//            }
//            (*_output) << prefix;
//            (*_output) << highlightResult;
//            (*_output) << postfix;
//            _currentHighlightStateData->currentState = _sourceHighlight.getHighlighter()->getCurrentState();
//            states.second = _currentHighlightStateData;
//            // reset the lastHighlightDelayedLine if necessary
//            if (_mode == Incremental && _currentLine == _lastHighlightDelayedLine) {
//                printf("resetting lastHighlightDelayedLine\n");
//                _lastHighlightDelayedLine.reset();
//            }
//            highlightedLine = true;
//        } else {
//            (*_output) << _htmlBuffer;
//            // if no need to highlight
//            // then we should pass through to the end state of the current line if possible
//            Q_ASSERT(_highlightStateDataHash.contains(*_currentLine));
//            _currentHighlightStateData = _highlightStateDataHash[*_currentLine].second;
//        }
//        _toHighlightBuffer.clear();
//    }
//    _htmlBuffer.clear();
//    // reset current line
//    // here we set it to a new string so that _lastHighlightDelayedLine
//    // will still point to the right line value
//    _currentLine = QStringPtr(new QString);
//    return highlightedLine;
//}

bool HtmlHighlight::replaceHtml(QTextStream &input, QTextStream &output, const QList<QPair<TextSelection, QString> > &replaces)
{
    if (replaces.count() == 0)
        return false;

    _replaces = replaces;

    return highlightHtmlBasic(input, output);
}

