/*
 *
 *  Created on: Feb 1, 2015
 *      Author: lingnan
 */

#ifndef REPLACEMENT_H_
#define REPLACEMENT_H_

#include <QDebug>

struct TextSelection {
    int start;
    int end;
    TextSelection(int _start, int _end): start(_start), end(_end) {}
    bool operator==(const TextSelection &other) const {
        return start == other.start && end == other.end;
    }
    bool operator!=(const TextSelection &other) const {
        return !operator==(other);
    }
};

struct Replacement {
    TextSelection selection;
    QString replacement;
    Replacement(TextSelection _selection, QString _replacement):
        selection(_selection), replacement(_replacement) {}
};

QDebug operator<<(QDebug dbg, const TextSelection &selection);
QDebug operator<<(QDebug dbg, const Replacement &replacement);

#endif /* REPLACEMENT_H_ */
