//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <fstream>

#include "sourcehighlight.h"
#include "formattermanager.h"
#include "textstyles.h"
#include "preformatter.h"
#include "parsestyles.h"
#include "textstyleformatter.h"
#include "textstyleformatterfactory.h"
#include "outlangdefparserfun.h"
#include "langdefmanager.h"
#include "regexrulefactory.h"
#include "highlightstate.h"
#include "sourcehighlighter.h"
#include "bufferedoutput.h"
#include "sourcefilehighlighter.h"
#include "linenumgenerator.h"
#include "ioexception.h"
#include "srcuntabifier.h"
#include "langmap.h"
#include "parserexception.h"
#include "highlightstateprinter.h"
#include "langelemsprinter.hpp"
#include "langelems.h"
#include "verbosity.h"
#include "settings.h"

using namespace std;

namespace srchilite {

SourceHighlight::SourceHighlight(const std::string &_outputLang) :
    outputLang(_outputLang), dataDir(Settings::retrieveDataDir()), styleFile(
            "default.style"), formatterManager(0), preFormatter(0),
            langDefManager(new LangDefManager(new RegexRuleFactory)),
            lineNumGenerator(0), highlightEventListener(0), lineRanges(0), regexRanges(0),
            optimize(true), generateLineNumbers(false), generateLineNumberRefs(false),
            lineNumberPad('0'), lineNumberDigits(0), binaryOutput(false),
            tabSpaces(0) {
}

SourceHighlight::~SourceHighlight() {
    if (formatterManager)
        delete formatterManager;

    if (preFormatter)
        delete preFormatter;

    delete langDefManager->getRuleFactory();
    delete langDefManager;

    if (lineNumGenerator)
        delete lineNumGenerator;
}

void SourceHighlight::initialize() {
    if (formatterManager)
        return; // already initialized

    TextStylesPtr textStyles = parse_outlang_def(dataDir.c_str(),
            outputLang.c_str());

    FormatterPtr defaultFormatter(new TextStyleFormatter("$text"));
    formatterManager = new FormatterManager(defaultFormatter);

    if (tabSpaces) {
        preFormatter = new Untabifier(tabSpaces);
        preFormatter->setPreFormatter(PreFormatterPtr(new PreFormatter(
                textStyles->charTranslator)));
    } else {
        preFormatter = new PreFormatter(textStyles->charTranslator);
    }

    linePrefix = textStyles->line_prefix;

    TextStyleFormatterFactory formatterFactory(textStyles, preFormatter, formatterManager);

    if (styleCssFile.size())
        parseCssStyles(dataDir, styleCssFile, &formatterFactory,
                backgroundColor);
    else
        parseStyles(dataDir, styleFile, &formatterFactory, backgroundColor);

    // keep the background color empty if none is specified
    if (backgroundColor != "")
        backgroundColor = formatterFactory.preprocessColor(backgroundColor);

    formatterFactory.addDefaultFormatter();

    // use the style default file to build missing formatters
    if (styleDefaultFile.size()) {
        LangMap defaultStyles(dataDir, styleDefaultFile);
        defaultStyles.open();
        for (LangMap::const_iterator it = defaultStyles.begin(); it
                != defaultStyles.end(); ++it) {
            formatterFactory.createMissingFormatter(it->first, it->second);
        }
    }

    formatterCollection = formatterFactory.getFormatterCollection();

    // initialize the line number generator
    TextStyleFormatter *lineNumFormatter =
            dynamic_cast<TextStyleFormatter *> (formatterManager->getFormatter(
                    "linenum").get());
    lineNumGenerator = new LineNumGenerator(lineNumFormatter->toString(), 5,
            lineNumberPad);
    lineNumGenerator->setAnchorPrefix(lineNumberAnchorPrefix);
    if (generateLineNumberRefs)
        lineNumGenerator->setAnchorStyle(textStyles->refstyle.anchor);

    // set the preformatter in all the formatters
    for (TextStyleFormatterCollection::const_iterator it =
            formatterCollection.begin(); it != formatterCollection.end(); ++it) {
        (*it)->setPreFormatter(preFormatter);
    }

    outputFileExtension = textStyles->file_extension;
}

void SourceHighlight::highlight(std::istream &input, std::ostream &output,
        const std::string &inputLang, const std::string &inputFileName) {

    initialize();

    HighlightStatePtr highlightState = langDefManager->getHighlightState(
            dataDir, inputLang);

    SourceHighlighter highlighter(highlightState);
    highlighter.setFormatterManager(formatterManager);
    highlighter.setOptimize(optimize);
    if (highlightEventListener)
        highlighter.addListener(highlightEventListener);

    BufferedOutput bufferedOutput(output);

    // if no optimization, then always flush the output
    if (!optimize)
        bufferedOutput.setAlwaysFlush(true);

    updateBufferedOutput(&bufferedOutput);

    SourceFileHighlighter fileHighlighter(inputFileName, &highlighter,
            &bufferedOutput);

    fileHighlighter.setLineRanges(lineRanges);
    fileHighlighter.setRegexRanges(regexRanges);

    if (generateLineNumbers) {
        fileHighlighter.setLineNumGenerator(lineNumGenerator);
        if (lineNumberDigits != 0) {
            lineNumGenerator->setDigitNum(lineNumberDigits);
        }
    }

    // set the prefix for all lines
    fileHighlighter.setLinePrefix(linePrefix);

    fileHighlighter.setPreformatter(preFormatter);

    // set the range separator only after the preformatter!
    // since the separator itself might have to be preformatted
    if (rangeSeparator.size()) {
        fileHighlighter.setRangeSeparator(rangeSeparator);
    }

    // the formatter for possible context lines
    fileHighlighter.setContextFormatter(formatterManager->getFormatter(
            "context").get());

    fileHighlighter.highlight(input);

    if (highlightEventListener)
        highlighter.removeListener(highlightEventListener);
}

void SourceHighlight::checkLangDef(const std::string &langFile) {
    // make sure to build the highlight state each time
    langDefManager->buildHighlightState(dataDir, langFile);

    // if we get here, no problems were found, otherwise this method
    // exits with an exception
}

void SourceHighlight::checkOutLangDef(const std::string &outlangFile) {
    // make sure to build the highlight state each time
    parse_outlang_def(dataDir.c_str(), outlangFile.c_str());

    // if we get here, no problems were found, otherwise this method
    // exits with an exception
}

void SourceHighlight::printHighlightState(const std::string &langFile,
        std::ostream &os) {
    HighlightStatePrinter printer(os);

    printer.printHighlightState(langDefManager->buildHighlightState(dataDir,
            langFile).get());
}

void SourceHighlight::printLangElems(const std::string &langFile,
        std::ostream &os) {
    LangElemsPrinter printer;
    LangElems *elems = langDefManager->getLangElems(dataDir, langFile);

    printer.print(elems, os);

    delete elems;
}

void SourceHighlight::updateBufferedOutput(BufferedOutput *output) {
    for (TextStyleFormatterCollection::const_iterator it =
            formatterCollection.begin(); it != formatterCollection.end(); ++it) {
        (*it)->setBufferedOutput(output);
    }
}

}
