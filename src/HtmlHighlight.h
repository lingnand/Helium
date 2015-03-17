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
#include <src/HtmlBufferChangeParser.h>
#include <src/srchilite/sourcehighlight.h>
#include <src/HighlightStateData.h>
#include <src/Type.h>
#include <src/BufferState.h>

typedef boost::shared_ptr<QString> QStringPtr;
typedef QPair<HighlightStateDataPtr, HighlightStateDataPtr> HighlightLineStates;

class HtmlHighlight
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
    QString _filetype;
    HighlightStateDataPtr _mainStateData;
    srchilite::SourceHighlight _sourceHighlight;
    HtmlBufferChangeParser _bufferChangeParser;
    // return true if the toHighlightBuffer is highlighted and result
    // appended to buffer, otherwise false
    bool highlightLine();
    bool highlightHtmlBasic(QTextStream &input, QTextStream &output);
    bool moreHighlightNeeded();
};

#endif /* HTMLHIGHLIGHT_H_ */
