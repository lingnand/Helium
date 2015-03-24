/*
 * type.h
 *
 *  Created on: Feb 1, 2015
 *      Author: lingnan
 */

#ifndef TYPE_H_
#define TYPE_H_

typedef QPair<int, int> TextSelection;

struct TextSelection {
    int start;
    int end;
    TextSelection(int _start, int _end): start(_start), end(_end) {}
};

struct Replacement {
    TextSelection selection;
    QString replacement;
    Replacement(TextSelection _selection, QString _replacement):
        selection(_selection), replacement(_replacement) {}
};

#endif /* TYPE_H_ */
