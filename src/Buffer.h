/*
 * Buffer.h
 *
 *  Created on: Nov 23, 2014
 *      Author: lingnan
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include <QObject>
#include <QUrl>
#include <boost/shared_ptr.hpp>
#include <src/srchilite/sourcehighlight.h>

namespace srchilite {
    class LangMap;
}

class HighlightStateData;

typedef boost::shared_ptr<HighlightStateData> HighlightStateDataPtr;
typedef boost::shared_ptr<QString> QStringPtr;
typedef QPair<HighlightStateDataPtr, HighlightStateDataPtr> HighlightLineStates;

class Buffer : public QObject
{
    Q_OBJECT
public:
    Buffer();
    virtual ~Buffer() {}
    // returns the name of the current buffer, "" for no name
    const QString &name() const { return _name; }
    const QString &filetype() const { return _filetype; }
    const QString &content() const { return _content; }
    // the name the buffer will use to write to the filesystem when requested
    Q_SLOT void setName(const QString &name);
    Q_SLOT void setFiletype(const QString &filetype);
    Q_SLOT void setContent(const QString &content, int cursorPosition, bool enableDelay);
Q_SIGNALS:
    // the name of the file changed
    void nameChanged(const QString& name);
    void filetypeChanged(const QString& filetype);
    void contentChanged(const QString& parsedContent);
private:
    bool _parsingContent;
    unsigned int _lineCounter;
    QHash<QString, HighlightLineStates> _highlightStateDataHash;
    QString _name;
    QString _filetype;
    QString _content;
    QStringPtr _lastHighlightDelayedLine;
    srchilite::SourceHighlight _sourceHighlight;
    srchilite::LangMap *_langMap;
    HighlightStateDataPtr _mainStateData;
    void parseContent(const QString content, int cursorPosition, bool noHighlight, bool enableDelay);
    bool tryHighlightContent(QString content, int cursorPosition, bool enableDelay);
};

#endif /* BUFFER_H_ */
