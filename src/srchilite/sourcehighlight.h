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

#include "textstyleformattercollection.h"
#include "sourcehighlighter.h"
#include "bufferedoutput.h"

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

    /// all the formatters that are created
    TextStyleFormatterCollection formatterCollection;

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
    SourceHighlight(const std::string &_styleFile, const std::string &_outputLang);
    ~SourceHighlight();
    /**
     * Only check the validity of the language definition file.
     * If the language definition is valid it simply returns, otherwise,
     * it throws an exception (with the details of the problems found)
     * @param langFile
     * @throws HighlightBuilderException
     */
    void checkLangDef(const std::string &langFile);

    /**
     * Only check the validity of the out language definition file.
     * If the language definition is valid it simply returns, otherwise,
     * it throws an exception (with the details of the problems found)
     * @param langFile
     * @throws ParserException
     */
    void checkOutLangDef(const std::string &langFile);

    /**
     * Prints the HighlightState corresponding to the language definition file.
     * If the language definition is valid it simply prints the state and returns, otherwise,
     * it throws an exception (with the details of the problems found)
     * @param langFile
     * @param os where to print the highlight state
     * @throws HighlightBuilderException
     */
    void printHighlightState(const std::string &langFile, std::ostream &os);

    /**
     * Prints the language elements corresponding to the language definition file.
     * If the language definition is valid it simply prints the state and returns, otherwise,
     * it throws an exception (with the details of the problems found)
     * @param langFile
     * @param os where to print the highlight state
     * @throws HighlightBuilderException
     */
    void printLangElems(const std::string &langFile, std::ostream &os);

    void clearBuffer();
    const std::ostringstream &getBuffer() {
        return buffer;
    }

    void setStyleFile(const std::string &_styleFile) {
        styleFile = _styleFile;
    }

    const TextStyleFormatterCollection &getFormatterCollection() const {
        return formatterCollection;
    }

    void setHighlightEventListener(HighlightEventListener *l) {
        highlightEventListener = l;
    }

    void setTabSpaces(unsigned int i) {
        tabSpaces = i;
    }

    void setInputLang(const std::string &inputLang);

    SourceHighlighter *getHighlighter() const {
        return highlighter;
    }
};

}

#endif /*SOURCEHIGHLIGHT_H_*/
