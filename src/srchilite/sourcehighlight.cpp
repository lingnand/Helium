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
#include "ioexception.h"
#include "srcuntabifier.h"
#include "langmap.h"
#include "parserexception.h"
#include "highlightstateprinter.h"
#include "langelemsprinter.hpp"
#include "langelems.h"
#include "settings.h"
#include "instances.h"

using namespace std;

namespace srchilite {

SourceHighlight::SourceHighlight(const std::string &_styleFile, const std::string &_outputLang) :
        dataDir(Settings::retrieveDataDir()), styleFile(_styleFile), output(buffer),
        formatterManager(0), preFormatter(0), langDefManager(Instances::getLangDefManager()),
            highlightEventListener(0), highlighter(0), tabSpaces(0) {
    TextStylesPtr textStyles = parse_outlang_def(dataDir.c_str(),
            _outputLang.c_str());

    FormatterPtr defaultFormatter(new TextStyleFormatter("$text"));
    formatterManager = new FormatterManager(defaultFormatter);

    if (tabSpaces) {
        preFormatter = new Untabifier(tabSpaces);
        preFormatter->setPreFormatter(PreFormatterPtr(new PreFormatter(
                textStyles->charTranslator)));
    } else {
        preFormatter = new PreFormatter(textStyles->charTranslator);
    }

    TextStyleFormatterFactory formatterFactory(textStyles, preFormatter, formatterManager);

    std::string backgroundColor;
    parseStyles(dataDir, styleFile, &formatterFactory, backgroundColor);

    formatterFactory.addDefaultFormatter();

    formatterCollection = formatterFactory.getFormatterCollection();

    // set the preformatter in all the formatters
    for (TextStyleFormatterCollection::const_iterator it =
            formatterCollection.begin(); it != formatterCollection.end(); ++it) {
        (*it)->setPreFormatter(preFormatter);
    }

    // wire up the output
    for (TextStyleFormatterCollection::const_iterator it =
            formatterCollection.begin(); it != formatterCollection.end(); ++it) {
        (*it)->setBufferedOutput(&output);
    }
}

SourceHighlight::~SourceHighlight() {
    if (highlighter)
        delete highlighter;

    if (formatterManager)
        delete formatterManager;

    if (preFormatter)
        delete preFormatter;
}

void SourceHighlight::setInputLang(const std::string &_inputLang) {
    if (!highlighter || _inputLang != inputLang) {
        delete highlighter;
        inputLang = _inputLang;

        if (inputLang != "") {
            HighlightStatePtr highlightState = langDefManager->getHighlightState(
                    dataDir, inputLang);

            highlighter = new SourceHighlighter(highlightState);
            highlighter->setFormatterManager(formatterManager);
            highlighter->setOptimize(false);
            if (highlightEventListener)
                highlighter->addListener(highlightEventListener);
        }
    }
}

void SourceHighlight::clearBuffer() {
    buffer.clear();
    buffer.str("");
}

}
