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
        if (_filetype.isEmpty()) {
            _mainStateData.reset();
        } else {
            _sourceHighlight.setInputLang(std::string(_filetype.toUtf8().constData()) + ".lang");
            // initialize the main state data
            _mainStateData = HighlightStateDataPtr(new HighlightStateData(
                    _sourceHighlight.getHighlighter()->getMainState(),
                    srchilite::HighlightStateStackPtr(new srchilite::HighlightStateStack())
            ));
        }
    }
}

HighlightStateDataPtr HtmlHighlight::highlightLine(BufferLine &line, HighlightStateDataPtr startState)
{
    if (filetype().isEmpty()) {
        line.setHighlightText(QString());
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

void HtmlHighlight::highlight(BufferState &state)
{
    state.setFiletype(filetype());
    HighlightStateDataPtr currentHighlightData = _mainStateData;
    for (int i = 0; i < state.size(); i++) {
        currentHighlightData = highlightLine(state[i], currentHighlightData);
    }
}

// returning: have we highlighted any change?
bool HtmlHighlight::highlightChange(BufferState &state, QTextStream &input, int cursorPosition, bool enableDelay)
{
    state.setCursorPosition(cursorPosition);
    state.setFiletype(filetype());
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
    return !filetype().isEmpty() && !enableDelay || !change.delayable();
}

void HtmlHighlight::replace(BufferState &state, const QList<QPair<TextSelection, QString> > &replaces)
{

}
