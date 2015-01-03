/*
 * stylefileparser.cpp
 *
 *  Created on: Dec 9, 2008
 *      Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2008
 *  Copyright: See COPYING file that comes with this distribution
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "stylefileparser.h"
#include "parsestyles.h"
#include "settings.h"

namespace srchilite {

void StyleFileParser::parseStyleFile(const std::string &name,
            FormatterFactory *formatterFactory, std::string &bodyBgColor) {
    parseStyles(name, formatterFactory, bodyBgColor);
}

void StyleFileParser::parseCssStyleFile(const std::string &name,
            FormatterFactory *formatterFactory, std::string &bodyBgColor) {
    parseCssStyles(Settings::retrieveDataDir(), name, formatterFactory, bodyBgColor);
}

void StyleFileParser::parseStyleFile(const std::string &path, const std::string &name,
            FormatterFactory *formatterFactory, std::string &bodyBgColor) {
    parseStyles(path, name, formatterFactory, bodyBgColor);
}

void StyleFileParser::parseCssStyleFile(const std::string &path, const std::string &name,
            FormatterFactory *formatterFactory, std::string &bodyBgColor) {
    parseCssStyles(path, name, formatterFactory, bodyBgColor);
}

}
