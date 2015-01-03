//
// C++ Implementation: SourceFileHighlighter
//
// Description:
//
//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <fstream>

#include "sourcefilehighlighter.h"

#include "sourcehighlighter.h"
#include "formatterparams.h"
#include "bufferedoutput.h"
#include "preformatter.h"
#include "formatter.h"
#include "linenumgenerator.h"
#include <sstream>
#include "ioexception.h"
#include "lineranges.h"
#include "regexranges.h"

using namespace std;

namespace srchilite {

typedef enum {
    FOUND_EOF = 0, FOUND_NL, FOUND_END
} load_line_ret;

static load_line_ret load_line(std::string& s, std::istream& is);

SourceFileHighlighter::SourceFileHighlighter(const std::string &file,
        SourceHighlighter *_sourceHighlighter, BufferedOutput *_output) :
    fileName(file), sourceHighlighter(_sourceHighlighter), output(_output),
            debugPolicy(NO_DEBUG), preformatter(0), lineNumGenerator(0),
            lineRanges(0), regexRanges(0), contextFormatter(0) {
}

load_line_ret load_line(std::string& s, std::istream& is) {
    s.erase();
    if (is.bad() || is.eof())
        return FOUND_EOF;

    char c;
    while (is.get(c)) {
        if (c == '\n')
            return FOUND_NL;
        if (c != '\r')
            s.append(1, c);
    }

    return FOUND_END;
}

void SourceFileHighlighter::setRangeSeparator(const std::string &rangeSep) {
    if (preformatter) {
        rangeSeparator = preformatter->preformat(rangeSep + "\n");
    } else {
        rangeSeparator = rangeSep + "\n";
    }
}

void SourceFileHighlighter::highlight() {
    istream *is = 0;

    if (fileName != "") {
        is = new ifstream(fileName.c_str());
        if (!is || !(*is)) {
            throw IOException("cannot open for input", fileName);
        }
    } else
        is = &cin;

    highlight(*is);

    if (fileName != "")
        delete is;
}

void SourceFileHighlighter::highlight(istream &is) {
    std::string s;

    FormatterParams params(fileName);
    params.line = 1;

    bool rangeSeparatorPrinted = false;

    sourceHighlighter->setFormatterParams(&params);
    sourceHighlighter->setSuspended(false);

    // if we have a LineRanges, make sure we reset it
    if (lineRanges) {
        lineRanges->reset();
    }

    if (regexRanges) {
        regexRanges->reset();
    }

    load_line_ret ret;
    RangeResult rangeResult = IN_RANGE;

    while ((ret = load_line(s, is)) != FOUND_EOF) {
        // first check whether we must highlight only a specific range of lines
        if (lineRanges) {
            rangeResult = lineRanges->isInRange(params.line);
            if (rangeResult == IN_RANGE) {
                sourceHighlighter->setSuspended(false);
                rangeSeparatorPrinted = false;
            } else {
                sourceHighlighter->setSuspended(true);
            }
        } else if (regexRanges) {
            // we assume that regex range is set only if line range is not set
            if (regexRanges->isInRange(s)) {
                rangeResult = IN_RANGE;
                sourceHighlighter->setSuspended(false);
            } else {
                rangeResult = NOT_IN_RANGE;
                sourceHighlighter->setSuspended(true);
            }
        }

        if (rangeResult != NOT_IN_RANGE) {
            output->output(linePrefix);

            if (lineNumGenerator) {
                output->output(lineNumGenerator->generateLine(params.line));
            }
        } else {
            // check whether we must print the range separator
            if (!rangeSeparatorPrinted && params.line != 1) {
                if (rangeSeparator.size()) {
                    output->output(linePrefix);
                    output->output(rangeSeparator);
                    rangeSeparatorPrinted = true;
                }
            }
        }

        if (rangeResult == CONTEXT_RANGE) {
            // the whole line must be formatted using the "context" style
            contextFormatter->format(s, &params);
        }

        // highlighting must be performed anyway, since this will keep track
        // of the current state; if we've set suspended, simply no output will be produced
        sourceHighlighter->highlightParagraph(s);

        if (rangeResult != NOT_IN_RANGE) {
            // format the newline
            if (ret == FOUND_NL) {
                // the newline char is not highlighted, but only generated
                // possibly after a transformation
                output->output((preformatter ? preformatter->preformat("\n")
                        : "\n"));
            }

            output->writePostLine(linePrefix);
        }

        (params.line)++;
    }

    output->writePostDoc(linePrefix);
}

void SourceFileHighlighter::highlight(const string &s) {
    istringstream is(s);
    highlight(is);
}

}
