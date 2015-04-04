/*
 * BufferState.h
 *
 *  Created on: Mar 8, 2015
 *      Author: lingnan
 */

#ifndef BUFFERSTATE_H_
#define BUFFERSTATE_H_

#include <QObject>
#include <QStringList>
#include <QTextStream>
#include <src/HighlightStateData.h>
#include <QDebug>

class BufferLine
{
    // text ready to be used inside <pre></pre>
public:
    BufferLine();
    virtual ~BufferLine() {}
    int size();
    bool isEmpty();
    void setHighlightText(const QString &highlightText);
    HighlightStateDataPtr endHighlightState();
    void setEndHighlightState(HighlightStateDataPtr endState);

    // line modification
    // split whatever is after the position into a new BufferLine
    BufferLine split(int position);
    void append(const BufferLine &other);
    void append(const QChar &);
    void append(const QString &);
    BufferLine &operator<<(const QChar &);
    BufferLine &operator<<(const QString &);
    BufferLine &operator<<(const BufferLine &);
    void swap(BufferLine &other);

    void writePlainText(QTextStream &output) const;
    void writePreText(QTextStream &output) const;
    void writeHighlightText(QTextStream &output) const;
    // convenience functions
    QString plainText() const;
    QString preText() const;
    QString highlightText() const;
private:
    int _size;
    QStringList _preTextSegments;
    QList<QChar> _specialChars;
    QString _highlightText;
    HighlightStateDataPtr _endHighlightState;
};

// a buffer state is a snapshot of the current buffer content
// it consists of a list of BufferLine's
// each line is identified by its index inside the BufferState

// on each new highlight iteration, the BufferState is modified
// by comparing the user-changed actual textArea content with the current BufferState,
// referencing the current one as much as possible and add/delete BufferLine's
// when necessary
class BufferState : public QList<BufferLine>
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
    void writeHighlightedHtml(QTextStream &output, int beginIndex = 0);
    // convenience function that returns a QString
    QString highlightedHtml(int beginIndex, int endIndex);
    QString highlightedHtml(int beginIndex = 0);
    const QString &filetype() const;
    int cursorPosition();
    void setCursorPosition(int cursorPosition);
    void setFiletype(const QString &filetype);
private:
    QString _filetype;
    int _cursorPosition;
};

QDebug operator<<(QDebug dbg, const BufferLine &line);

#endif /* BUFFERSTATE_H_ */
