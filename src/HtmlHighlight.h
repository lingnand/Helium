/*
 * HtmlHighlight.h
 *
 *  Created on: Jan 14, 2015
 *      Author: lingnan
 */

#ifndef HTMLHIGHLIGHT_H_
#define HTMLHIGHLIGHT_H_

#include <src/HtmlParser.h>
#include <src/srchilite/sourcehighlight.h>
#include <src/HighlightStateData.h>

typedef boost::shared_ptr<QString> QStringPtr;
typedef QPair<HighlightStateDataPtr, HighlightStateDataPtr> HighlightLineStates;

class HtmlHighlight : public HtmlParser
{
    Q_OBJECT
public:
    HtmlHighlight(const QString &style);
    virtual ~HtmlHighlight() {}
    const QString &filetype() const { return _filetype; }
    QString highlightHtml(const QString &html, int cursorPosition, bool enableDelay);
    Q_SLOT void setFiletype(const QString &filetype);
Q_SIGNALS:
    void filetypeChanged(const QString &filetype);
private:
    unsigned int _lineCounter;
    enum ToHighlightState { NoHighlight = 0, HighlightCurrent = 1, HighlightDelayed = 2};
    bool _highlighted;
    bool _stopParsing;
    bool _enableDelay;
    bool _afterTTTag;
    bool _reachedCursor;
    unsigned int _cursorPosition;
    ToHighlightState _toHighlight;
    QString _filetype;
    QString _buffer;
    QString _toHighlightBuffer;
    QString _htmlBuffer;
    QStringPtr _lastHighlightDelayedLine;
    QStringPtr _currentLine;
    HighlightStateDataPtr _currentHighlightStateData;
    HighlightStateDataPtr _mainStateData;
    QHash<QString, HighlightLineStates> _highlightStateDataHash;
    srchilite::SourceHighlight _sourceHighlight;
    bool stopParsing();
    void parseCharacter(const QChar &ch, unsigned int charCount);
    void parseTag(const QString &name, const QString &attributeName, const QString &attributeValue);
    void parseHtmlCharacter(const QChar &ch);
    void reachedEnd();
    // return true if the toHighlightBuffer is highlighted and result
    // appended to buffer, otherwise false
    bool highlightLine();
};

#endif /* HTMLHIGHLIGHT_H_ */
