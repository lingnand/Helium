/*
 * BufferState.h
 *
 *  Created on: Mar 8, 2015
 *      Author: lingnan
 */

#ifndef BUFFERSTATE_H_
#define BUFFERSTATE_H_

#include <QObject>
#include <QTextStream>
#include <src/HighlightStateData.h>

class BufferLine
{
    // text ready to be used inside <pre></pre>
public:
    BufferLine();
    virtual ~BufferLine() {}
    int charCount();
    void setHighlightText(const QString &highlightText);
    HighlightStateDataPtr beginHighlightState();
    HighlightStateDataPtr endHighlightState();
    void setBeginHighlightState(HighlightStateDataPtr beginState);
    void setEndHighlightState(HighlightStateDataPtr endState);

    BufferLine &BufferLine::operator<<(QChar c);

    void writePlainText(QTextStream &output);
    void writePreText(QTextStream &output);
    void writeHighlightText(QTextStream &output);
private:
    int _charCount;
    QStringList _preTextSegments;
    QList<QChar> _specialChars;
    QString _highlightText;
    HighlightStateDataPtr _beginHighlightState;
    HighlightStateDataPtr _endHighlightState;
};
// a buffer state is a snapshot of the current buffer content
// it consists of a list of BufferLinePtr's
// each line is identified by its index inside the BufferState

// on each new highlight iteration, the BufferState is modified
// by comparing the user-changed actual textArea content with the current BufferState,
// referencing the current one as much as possible and add/delete BufferLine's
// when necessary
class BufferState : QList<BufferLine>
{
public:
    // return the index of the line where the cursor is currently in
    // NOTE: the cursorPosition is assumed to be based on plainText
    int focusedLine(int cursorPosition);
    void writePlainText(QTextStream &output);
    void writePartialHighlightedHtml(QTextStream &output, int highlightStartLine, int highlightEndLine);
    QString &filetype();
    void setFiletype(QString &filetype);
private:
    QString _filetype;
};

#endif /* BUFFERSTATE_H_ */
