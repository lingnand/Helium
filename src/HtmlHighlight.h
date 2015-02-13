/*
 * HtmlHighlight.h
 *
 *  Created on: Jan 14, 2015
 *      Author: lingnan
 */

#ifndef HTMLHIGHLIGHT_H_
#define HTMLHIGHLIGHT_H_

#include <boost/regex.hpp>
#include <src/HtmlParser.h>
#include <src/srchilite/sourcehighlight.h>
#include <src/HighlightStateData.h>
#include <src/Type.h>

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
    QString replaceHtml(const QString &html, const QList<QPair<TextSelection, QString> > &replaces);
    Q_SLOT void setFiletype(const QString &filetype);
    Q_SLOT void clearHighlightStateDataHash();
Q_SIGNALS:
    void filetypeChanged(const QString &filetype);
private:
    int _lineCounter;
    enum ToHighlightState { NoHighlight = 0, HighlightCurrent = 1, HighlightDelayed = 2};
    enum HighlightMode { Incremental, Replace };
    // Incremental mode variables
    bool _enableDelay;
    bool _reachedCursor;
    int _cursorPosition;
    // Replace mode variables
    QList<QPair<TextSelection, QString> > _replaces;
    HighlightMode _mode;
    bool _highlighted;
    bool _stopParsing;
    bool _afterTTTag;
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
    void parseCharacter(const QChar &ch, int charCount);
    void parseTag(const QString &name, const QString &attributeName, const QString &attributeValue);
    void parseHtmlCharacter(const QChar &ch);
    void reachedEnd();
    // return true if the toHighlightBuffer is highlighted and result
    // appended to buffer, otherwise false
    bool highlightLine();
    bool highlightHtmlBasic(const QString &html);
    bool moreHighlightNeeded();
};

#endif /* HTMLHIGHLIGHT_H_ */
