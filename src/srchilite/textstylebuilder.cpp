/**
 * C++ class: textstylebuilder.h
 *
 * Description: Given TextStyle objects build a new one,
 * adding a starting part, and ending part, and separating
 * them.
 *
 * Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2005
 * Copyright: See COPYING file that comes with this distribution
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "textstylebuilder.h"

using namespace std;

namespace srchilite {

TextStyleBuilder::TextStyleBuilder(const std::string &st,
        const std::string &sep) :
    start_(st), separator_(sep), added(false) {
}

void TextStyleBuilder::start() {
    buffer = TextStyle(start_);
    added = false;
}

void TextStyleBuilder::add(const TextStyle &textStyle) {
    if (textStyle.empty())
        return;

    string separator = (added ? separator_ : "");
    if (buffer.containsStyleVar()) {
        buffer.update(TEXT_VAR_TEXT, separator + textStyle.toString());
        added = true;
    } else {
        buffer.update(separator + textStyle.toString());
    }
}

TextStyle TextStyleBuilder::end() {
    return TextStyle(buffer.toString());
}

}
