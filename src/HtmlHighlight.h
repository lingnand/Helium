/*
 * HtmlHighlight.h
 *
 *  Created on: Jan 14, 2015
 *      Author: lingnan
 */

#ifndef HTMLHIGHLIGHT_H_
#define HTMLHIGHLIGHT_H_

#include <QTextStream>
#include <boost/regex.hpp>
#include <src/HtmlParser.h>
#include <src/srchilite/sourcehighlight.h>
#include <src/HighlightStateData.h>
#include <src/Type.h>
#include <src/BufferState.h>

typedef boost::shared_ptr<QString> QStringPtr;
typedef QPair<HighlightStateDataPtr, HighlightStateDataPtr> HighlightLineStates;

class HtmlHighlight : public HtmlParser
{
    Q_OBJECT
public:
    HtmlHighlight(const QString &style);
    virtual ~HtmlHighlight() {}
    const QString &filetype() const { return _filetype; }
    bool highlightHtml(QTextStream &input, QTextStream &output, int cursorPosition, bool enableDelay);
    bool replaceHtml(QTextStream &input, QTextStream &output, const QList<QPair<TextSelection, QString> > &replaces);
    Q_SLOT void setFiletype(const QString &filetype);
    Q_SLOT void clearHighlightStateDataHash();
Q_SIGNALS:
    void filetypeChanged(const QString &filetype);
private:
    int _lineCounter;
    enum ToHighlightState { NoHighlight = 0, HighlightCurrent = 1, HighlightDelayed = 2};
    // Incremental mode variables
    bool _enableDelay;
    bool _reachedCursor;
    int _cursorPosition;
    // Replace mode variables
    QList<QPair<TextSelection, QString> > _replaces;
    bool _highlighted;
    bool _startedParsing;
    bool _stopParsing;
    bool _afterTTTag;
    ToHighlightState _toHighlight;
    QString _filetype;
    BufferState *_state;
    QString _toHighlightBuffer;
    int _lastHighlightDelayedLineIndex;
    int _currentIndex;

    HighlightStateDataPtr _currentHighlightStateData;
    HighlightStateDataPtr _mainStateData;
    srchilite::SourceHighlight _sourceHighlight;
    // return true if the toHighlightBuffer is highlighted and result
    // appended to buffer, otherwise false
    bool highlightLine();
    bool highlightHtmlBasic(QTextStream &input, QTextStream &output);
    bool moreHighlightNeeded();
};

#endif /* HTMLHIGHLIGHT_H_ */
