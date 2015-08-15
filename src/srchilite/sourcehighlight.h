//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef SOURCEHIGHLIGHT_H_
#define SOURCEHIGHLIGHT_H_

#include <string>
#include <istream>
#include <ostream>
#include <sstream>

#include "sourcehighlighter.h"
#include "bufferedoutput.h"
#include "textstyles.h"

namespace srchilite {

class FormatterManager;
class PreFormatter;
class LangDefManager;
class LineNumGenerator;
class CharTranslator;
class HighlightEventListener;
class LineRanges;
class RegexRanges;

/**
 * The main class performing highlighting of an input file generating
 * an output file.
 */
class SourceHighlight {
    /**
     * Path for several configuration files.
     * By default it contains the absolute data dir corresponding to the installation
     * path, e.g., "$prefix/share/source-highlight"
     */
    std::string dataDir;

    /// the style file
    std::string styleFile;

    /// the input lang for the output document
    std::string inputLang;

    /// the output
    std::ostringstream buffer;
    BufferedOutput output;

    /// the formatter manager
    FormatterManager *formatterManager;

    /// the preformatter
    PreFormatter *preFormatter;

    // the text styles
    TextStylesPtr textStyles;

    /// for loading language definitions
    LangDefManager *langDefManager;

    /**
     * The listener for highlight events
     */
    HighlightEventListener *highlightEventListener;

    /**
     * The actual source highlight object
     */
    SourceHighlighter *highlighter;

    /**
     * If greater than 0 it means that tabs will be replaced by tabSpaces
     * blank characters
     */
    unsigned int tabSpaces;

public:
    SourceHighlight(const std::string &_outputLang);
    ~SourceHighlight();

    void clearBuffer();
    const std::ostringstream &getBuffer() {
        return buffer;
    }

    FormatterManager *getFormatterManager() const {
        return formatterManager;
    }

    void setHighlightEventListener(HighlightEventListener *l) {
        highlightEventListener = l;
    }

    void setTabSpaces(unsigned int i) {
        tabSpaces = i;
    }

    void setInputLang(const std::string &inputLang);

    void setStyleFile(const std::string &styleFile);

    SourceHighlighter *getHighlighter() const {
        return highlighter;
    }
};

}

#endif /*SOURCEHIGHLIGHT_H_*/
