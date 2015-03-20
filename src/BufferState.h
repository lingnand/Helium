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
    bool isEmpty();
    void setHighlightText(const QString &highlightText);
    HighlightStateDataPtr endHighlightState();
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
// it consists of a list of BufferLine's
// each line is identified by its index inside the BufferState

// on each new highlight iteration, the BufferState is modified
// by comparing the user-changed actual textArea content with the current BufferState,
// referencing the current one as much as possible and add/delete BufferLine's
// when necessary
class BufferState : QList<BufferLine>
{
public:
    BufferState();
    virtual ~BufferState() {}
    // return the index of the line where the cursor is currently in
    // NOTE: the cursorPosition is assumed to be based on plainText
    int focus(int cursorPosition);
    void writePlainText(QTextStream &output);
    void writePreText(QTextStream &output);
    // beginIndex should always be smaller than endIndex
    void writeHighlightedHtml(QTextStream &output, int beginIndex, int endIndex);
    QString &filetype();
    int cursorPosition();
    void setCursorPosition(int cursorPosition);
    void setFiletype(QString &filetype);
private:
    QString _filetype;
    int _cursorPosition;
};

class BufferHistory : QList<BufferState>
{
public:
    // anything equal or below 0 means no limitation on the size
    BufferHistory(int upperLimit = 0);
    virtual ~BufferHistory() {}
    // this will remove all items after current
    BufferState &copyCurrent();
    BufferState &current();
    bool advance();
    bool retract();
    bool advanceable();
    bool retractable();
Q_SIGNALS:
    void advanceableChanged(bool advanceable);
    void retractableChanged(bool retractable);
private:
    int _upperLimit;
    int _currentIndex;
};

#endif /* BUFFERSTATE_H_ */
