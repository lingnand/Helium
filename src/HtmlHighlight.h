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
    const QString &filetype();
    Q_SLOT void setFiletype(const QString &filetype);
    bool highlightChange(BufferState &state, QTextStream &input, int cursorPosition, bool enableDelay)
    void highlight(BufferState &state);
    void replace(BufferState &state, const QList<QPair<TextSelection, QString> > &replaces);
Q_SIGNALS:
    void filetypeChanged(QString &filetype);
private:
    QString _filetype;
    HighlightStateDataPtr _mainStateData;
    srchilite::SourceHighlight _sourceHighlight;
    HtmlBufferChangeParser _bufferChangeParser;
    HighlightStateDataPtr highlightLine(BufferLine &line, HighlightStateDataPtr startState);
};

#endif /* HTMLHIGHLIGHT_H_ */
