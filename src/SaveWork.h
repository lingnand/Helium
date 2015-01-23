/*
 * SaveWork.h
 *
 *  Created on: Jan 19, 2015
 *      Author: lingnan
 */

#ifndef SAVEWORK_H_
#define SAVEWORK_H_

#include <src/BufferWork.h>

class HtmlPlainTextExtractor;

class SaveWork : public BufferWork
{
    Q_OBJECT
public:
    SaveWork(HtmlPlainTextExtractor *extractor);
    void setHtml(const QString &html);
    void run();
protected:
    QString _html;
    HtmlPlainTextExtractor *_extractor;
};

typedef boost::shared_ptr<BufferWork> SaveWorkPtr;

#endif /* SAVEWORK_H_ */
