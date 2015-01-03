/**
 * C++ class: textstyleformatter.cpp
 *
 * Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2005-2008
 * Copyright: See COPYING file that comes with this distribution
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "textstyleformatter.h"
#include "bufferedoutput.h"
#include "preformatter.h"
#include "wordtokenizer.h"
#include <sstream>

using namespace std;

namespace srchilite {

TextStyleFormatter::TextStyleFormatter(const TextStyle &style, BufferedOutput *o) :
    textstyle(style), output(o), preFormatter(0) {
}

TextStyleFormatter::TextStyleFormatter(const string &repr, BufferedOutput *o) :
    textstyle(TextStyle(repr)), output(o), preFormatter(0) {
}

void TextStyleFormatter::format(const string &s, const FormatterParams *params) {
    if (!s.size())
        return;

    if (preFormatter) {
        output->output(textstyle.output(preFormatter->preformat(s)));
    } else {
        output->output(textstyle.output(s));
    }
}

}
