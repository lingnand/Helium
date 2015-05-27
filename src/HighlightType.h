/*
 * HighlightType.h
 *
 *  Created on: May 26, 2015
 *      Author: lingnan
 */

#ifndef HIGHLIGHTTYPE_H_
#define HIGHLIGHTTYPE_H_

class Filetype;

struct HighlightType {
    Filetype *filetype;
    bool highlightEnabled;
    static HighlightType fromFiletype(Filetype *ft);
    HighlightType(Filetype *ft=NULL, bool highlight=false):
        filetype(ft), highlightEnabled(highlight) {}
    bool operator==(const HighlightType &other) const {
        return filetype == other.filetype &&
                highlightEnabled == other.highlightEnabled;
    }
    bool operator!=(const HighlightType &other) const {
        return !operator==(other);
    }
    bool shouldHighlight() const {
        return filetype && highlightEnabled;
    }
};
Q_DECLARE_METATYPE(HighlightType)


#endif /* HIGHLIGHTTYPE_H_ */
