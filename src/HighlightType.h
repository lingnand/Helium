/*
 * HighlightType.h
 *
 *  Created on: May 26, 2015
 *      Author: lingnan
 */

#ifndef HIGHLIGHTTYPE_H_
#define HIGHLIGHTTYPE_H_

#include <Filetype.h>

struct HighlightType {
    QString styleFile;
    Filetype *filetype;
    bool highlightEnabled;
    HighlightType(const QString &style, Filetype *ft, bool highlight):
        styleFile(style), filetype(ft), highlightEnabled(highlight) {}
    HighlightType(const QString &style=QString(), Filetype *ft=NULL):
        styleFile(style), filetype(ft), highlightEnabled(ft ? ft->highlightEnabled() : false) {}
    bool operator==(const HighlightType &other) const {
        return styleFile == other.styleFile &&
                filetype == other.filetype &&
                highlightEnabled == other.highlightEnabled;
    }
    bool operator!=(const HighlightType &other) const {
        return !operator==(other);
    }
    bool shouldHighlight() const {
        return filetype && highlightEnabled;
    }
};

#endif /* HIGHLIGHTTYPE_H_ */
