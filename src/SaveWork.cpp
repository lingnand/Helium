/*
 * SaveWork.cpp
 *
 *  Created on: Jan 19, 2015
 *      Author: lingnan
 */

#include <src/SaveWork.h>
#include <stdio.h>

SaveWork::SaveWork(): BufferWork(Save)
{
}

void SaveWork::run()
{
    // get the original text of the content
    QString toSave =_extractor.extractPlainText(_html);
    printf("toSave: %s\n", qPrintable(toSave));
    int max = 1000;
    int msec = 3000;
    int i = 0;
    while (i < max) {
        i++;
        emit inProgressChanged(i/(float)max);
        usleep(msec);
    }
    emit inProgressChanged(0);
}

void SaveWork::setHtml(const QString &html)
{
    _html = html;
}
