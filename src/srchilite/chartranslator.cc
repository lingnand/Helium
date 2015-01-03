/*
 * Copyright (C) 1999-2009  Lorenzo Bettini, http://www.lorenzobettini.it
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "chartranslator.h"
#include <sstream>

using namespace std;

namespace srchilite {

CharTranslator::CharTranslator(PreFormatterPtr f) :
    PreFormatter(f), counter(0), reg_exp(0), bol(true) {
}

CharTranslator::~CharTranslator() {
    if (reg_exp)
        delete reg_exp;
}

void CharTranslator::set_translation(const std::string &to_translate,
        const std::string &translate_into) {
    // here we only buffer the translation regular expression
    ostringstream exp;
    exp << (translation_exp.size() ? "|" : "") << "(" << to_translate << ")";

    translation_exp += exp.str();

    ostringstream format;
    // the translation format corresponding to "to_translate"
    format << "(?" << ++counter << translate_into << ")";

    translation_format += format.str();
}

const string CharTranslator::doPreformat(const string &text) {
    if (!translation_exp.size()) {
        return text;
    }

    // we finally build the actual regular expression
    if (!reg_exp)
        reg_exp = new boost::regex(translation_exp);

    boost::match_flag_type flags = boost::match_default | boost::format_all;
    if (!bol)
        flags |= boost::match_not_bol;
    // if we're not at the beginning of the line, then we must not match the
    // beginning of the string as the beginning of a line

    std::ostringstream preformat_text(std::ios::out | std::ios::binary);
    std::ostream_iterator<char, char> oi(preformat_text);
    boost::regex_replace(oi, text.begin(), text.end(), *reg_exp,
            translation_format, flags);

    // keep track of the fact that we begin a new line
    if (text.find('\n') != string::npos)
        bol = true;
    else
        bol = false;

    return preformat_text.str();
}

}
