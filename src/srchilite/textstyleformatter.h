/**
 * C++ class: textstyleformatter.h
 *
 * Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2005-2008
 * Copyright: See COPYING file that comes with this distribution
 */

#ifndef _TEXTSTYLEFORMATTER_H_
#define _TEXTSTYLEFORMATTER_H_

#include "textstyle.h"
#include "formatter.h"

namespace srchilite {

class BufferedOutput;
class PreFormatter;

/**
 * An implementation of Formatter, based on source-highlight TextStyle
 */
class TextStyleFormatter : public Formatter {
private:
    /// the TextStyle for formatting
    TextStyle textstyle;

    /// where to output the formatting
    BufferedOutput *output;

    /// the possible pre formatter for text
    PreFormatter *preFormatter;

public:
    TextStyleFormatter(const TextStyle &style, BufferedOutput *o = 0);
            TextStyleFormatter(const std::string &repr = "$text",
                    BufferedOutput *o = 0);

    virtual ~TextStyleFormatter() {
    }

    virtual void
            format(const std::string &s, const FormatterParams *params = 0);

    BufferedOutput *getBufferedOutput() const {
        return output;
    }

    void setBufferedOutput(BufferedOutput *o) {
        output = o;
    }

    void setPreFormatter(PreFormatter *p) {
        preFormatter = p;
    }

    /**
     * @return a string representation of this formatter
     */
    const std::string &toString() const {
        return textstyle.toString();
    }

};

}

#endif /* _TEXTSTYLEFORMATTER_H_ */
