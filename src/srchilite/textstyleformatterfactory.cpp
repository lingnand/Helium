//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "textstyleformatterfactory.h"
#include "formattermanager.h"
#include "textstylebuilder.h"
#include "textstyleformatter.h"
#include "keys.h"

namespace srchilite {

TextStyleFormatterFactory::TextStyleFormatterFactory(TextStylesPtr _textStyles,
        PreFormatter *_preformatter, FormatterManager *_formatterManager) :
    textStyles(_textStyles), preformatter(_preformatter), formatterManager(_formatterManager) {
}

TextStyleFormatterFactory::~TextStyleFormatterFactory() {
}

bool TextStyleFormatterFactory::createFormatter(const string &key,
        const string &color, const string &bgcolor,
        StyleConstantsPtr styleconstants) {
    if (formatterManager->hasFormatter(key).get())
        return false;

    if (!textStyles->onestyle.empty()) {
        TextStyleFormatter *textStyleFormatter = new TextStyleFormatter(textStyles->onestyle.subst_style(key));
        formatterManager->addFormatter(key, FormatterPtr(textStyleFormatter));
        formatterCollection.push_back(textStyleFormatter);

        return true;
    }

    TextStyleBuilder textStyleBuilder(textStyles->starting_template,
            textStyles->style_separator);

    textStyleBuilder.start();

    if (key == "linenum") {
        // for line numbers we have the style for the container of the linenum
        // and the style of the line number (e.g., the line number can be
        // generated in bold, and inside a table column)
        textStyleBuilder.add(textStyles->linenum);
    }

    if (styleconstants.get()) {
        for (StyleConstantsIterator it = styleconstants->begin(); it
                != styleconstants->end(); ++it) {
            switch (*it) {
            case ISBOLD:
                textStyleBuilder.add(textStyles->bold);
                break;
            case ISITALIC:
                textStyleBuilder.add(textStyles->italics);
                break;
            case ISUNDERLINE:
                textStyleBuilder.add(textStyles->underline);
                break;
            case ISFIXED:
                textStyleBuilder.add(textStyles->fixed);
                break;
            case ISNOTFIXED:
                textStyleBuilder.add(textStyles->notfixed);
                break;
            case ISNOREF:
                break;
            }
        }
    }

    if (color.size()) {
        textStyleBuilder.add(textStyles->color.subst_style(preprocessColor(color)));
    }

    if (bgcolor.size()) {
        textStyleBuilder.add(textStyles->bg_color.subst_style(preprocessColor(bgcolor)));
    }

    TextStyle style = textStyleBuilder.end();

    TextStyleFormatter *textStyleFormatter = new TextStyleFormatter(style);

    formatterManager->addFormatter(key, FormatterPtr(textStyleFormatter));
    formatterCollection.push_back(textStyleFormatter);
    return true;
}

string TextStyleFormatterFactory::preprocessColor(const string &color) {
    if (color[0] == '"' && color[color.size()-1] == '"')
        return color.substr(1, color.size()-2);
    else
        return textStyles->colorMap->getColor(color);
}

bool TextStyleFormatterFactory::createMissingFormatter(const string &key1,
        const string &key2) {
    // the formatter for key1 is not missing
    if (formatterManager->hasFormatter(key1).get())
        return false;

    // the formatter for key2 is missing
    if (!formatterManager->hasFormatter(key2).get())
        return false;

    // the formatter for key1 will have the same style as the formatter for key2
    formatterManager->addFormatter(key1, formatterManager->getFormatter(key2));

    // there's no need to update the formatterCollection, since no new formatter was created

    return true;
}

void TextStyleFormatterFactory::addDefaultFormatter() {
    FormatterPtr defaultFormatter = formatterManager->hasFormatter(NORMAL);

    // if the style for normal was not specified
    if (!defaultFormatter.get()) {
        TextStyleFormatter *textStyleFormatter = 0;

        if (textStyles->onestyle.empty())
            textStyleFormatter = new TextStyleFormatter();
        else
            textStyleFormatter = new TextStyleFormatter(textStyles->onestyle.subst_style(NORMAL));

        defaultFormatter = FormatterPtr(textStyleFormatter);
        formatterManager->addFormatter(NORMAL, defaultFormatter);
        formatterCollection.push_back(textStyleFormatter);
    } else {
        formatterCollection.push_back(dynamic_cast<TextStyleFormatter *>(defaultFormatter.get()));
    }

    formatterManager->setDefaultFormatter(defaultFormatter);
}

}
