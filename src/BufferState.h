/*
 * BufferState.h
 *
 *  Created on: Mar 8, 2015
 *      Author: lingnan
 */

#ifndef BUFFERSTATE_H_
#define BUFFERSTATE_H_

#include <QStringList>
#include <QTextStream>
#include <HighlightStateData.h>
#include <HtmlParser.h>
#include <QDebug>

struct Range {
    int from;
    int to;
    bool operator==(const Range &other) {
        return from == other.from && to == other.to;
    }
    bool operator!=(const Range &other) {
        return !operator==(other);
    }
    // if the current range is a subset of the other range
    bool operator<=(const Range &other) {
        return from >= other.from && to <= other.to;
    }
    bool operator>=(const Range &other) {
        return from <= other.from && to >= other.to;
    }
    Range &grow(int diff) {
        if (diff >= from)
            from = 0;
        else
            from -= diff;
        if (INT_MAX - to <= diff) {
            to = INT_MAX;
        } else {
            to += diff;
        }
        return *this;
    }
    Range &clamp(int fromLimit, int toLimit) {
        from = qMin(qMax(from, fromLimit), to);
        to = qMax(qMin(to, toLimit), from);
        return *this;
    }
    Range(int _from=0): from(_from), to(_from) {}
    Range(int _from, int _to): from(_from), to(_to) {}
};

class BufferLine
{
    // text ready to be used inside <pre></pre>
public:
    BufferLine();
    virtual ~BufferLine() {}
    int size() const;
    int preTextSize() const;
    bool isEmpty() const;
    void clear();

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
    // convenience functions
    QString plainText() const;
    QString preText() const;
private:
    int _1size; // number of characters that will translate into 1 char in pretext
    int _4size; // ... translate into 4 chars e.g. &lt;
    int _5size; // .. translate into 5 chars e.g. &amp;
    QStringList _preTextSegments;
    QList<QChar> _specialChars;
};

struct BufferLineState {
    BufferLine line;
    QString highlightText;
    HighlightStateData::ptr endHighlightState;
    BufferLineState(BufferLine _line = BufferLine(), HighlightStateData::ptr _endHighlightState = HighlightStateData::ptr()):
        line(_line), endHighlightState(_endHighlightState) {}
    // return the size diff between html and plaintext
    int diff() const {
        return highlightText.size() - line.size();
    }
};


class Filetype;

// a buffer state is a snapshot of the current buffer content
// it consists of a list of BufferLine's
// each line is identified by its index inside the BufferState

// on each new highlight iteration, the BufferState is modified
// by comparing the user-changed actual textArea content with the current BufferState,
// referencing the current one as much as possible and add/delete BufferLine's
// when necessary
class BufferState : public QList<BufferLineState>
{
public:
    BufferState(Filetype *filetype=NULL, int cursorPosition=0);
    virtual ~BufferState() {}
    struct Position {
        int lineIndex;
        int linePosition;
        Position(): lineIndex(-1), linePosition(-1) {}
    };
    // return the cursorPosition at the beginning of the specified line
    int cursorPositionAtLine(int line) const;
    // return the index of the line where the cursor is currently in
    // NOTE: the cursorPosition is assumed to be based on plainText
    Position focus(int cursorPosition) const;
    QString plainText() const;
    // TODO: int should be enough, shouldn't it?
    int plainTextSize() const;
    // beginIndex should always be smaller than endIndex
    // return: the parser position at the beginning of the highlight section
    ParserPosition writeHighlightedHtml(QTextStream &output, const Range &) const;
    ParserPosition writeHighlightedHtml(QTextStream &output, int beginIndex, int endIndex) const;
    ParserPosition writeHighlightedHtml(QTextStream &output, int beginIndex = 0) const;
    Filetype *filetype() const { return _filetype; }
    void setFiletype(Filetype *filetype) { _filetype = filetype; }
    int cursorPosition() const { return _cursorPosition; }
    void setCursorPosition(int cursorPosition) { _cursorPosition = cursorPosition; }
private:
    Filetype *_filetype;
    int _cursorPosition;
};
Q_DECLARE_METATYPE(BufferState)

QDebug operator<<(QDebug dbg, const BufferLine &line);
QDebug operator<<(QDebug dbg, const BufferLineState &lineState);
QDebug operator<<(QDebug dbg, const Range &range);
QDebug operator<<(QDebug dbg, const BufferState::Position &pos);

#endif /* BUFFERSTATE_H_ */
