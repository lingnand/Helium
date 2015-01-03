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
#include <src/srchilite/sourcehighlight.h>

class Buffer : public QObject
{
    Q_OBJECT
public:
    Buffer();
    virtual ~Buffer() {}
    // returns the name of the current buffer, "" for no name
    const QString& name() const;
    const QUrl& imageSource() const;
    const QString& parsedContent() const;
    Q_SLOT void setName(const QString& name);
    Q_SLOT void setContent(const QString& content);
Q_SIGNALS:
    // the name of the file changed
    void nameChanged(const QString& name);
    // the icon of the file changed
    void imageSourceChanged(const QUrl& imageSource);
    // new parsed output arrived
    void parsedContentChanged(const QString& parsedContent);
private:
    QString m_pName;
    QUrl m_pImageSource;
    QString m_pParsedContent;
    srchilite::SourceHighlight m_pSourceHighlight;
    const QString& parseContent(const QString& content);
};

#endif /* BUFFER_H_ */
