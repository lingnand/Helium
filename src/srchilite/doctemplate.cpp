/**
 * C++ class: doctemplate.h
 *
 * Description: The template for a document containing the output of
 * highlighting
 *
 * Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2005-2007
 * Copyright: See COPYING file that comes with this distribution
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "doctemplate.h"
#include "substfun.h"

namespace srchilite {

#define TITLE_VAR_TEXT "$title" // the text of the title variable
#define INPUT_LANG_VAR_TEXT "$inputlang" // the text of the input lang variable
#define CSS_VAR_TEXT "$css" // the text of the css variable
#define ADDITIONAL_VAR_TEXT "$additional" // the text of the additional text variable
#define HEADER_VAR_TEXT "$header" // the text of the header variable
#define FOOTER_VAR_TEXT "$footer" // the text of the footer variable
#define BACKGROUND_VAR_TEXT "$docbgcolor" // the text of the background variable
#define TITLE_VAR "\\" TITLE_VAR_TEXT // the name of the title variable as regexp
#define INPUT_LANG_VAR "\\" INPUT_LANG_VAR_TEXT // the name of the input lang variable as regexp
#define CSS_VAR "\\" CSS_VAR_TEXT // the name of the css variable as regexp
#define ADDITIONAL_VAR "\\" ADDITIONAL_VAR_TEXT // the text of the additional text variable as regexp
#define HEADER_VAR "\\" HEADER_VAR_TEXT // the text of the header variable as regexp
#define FOOTER_VAR "\\" FOOTER_VAR_TEXT // the text of the footer variable as regexp
#define BACKGROUND_VAR "\\" BACKGROUND_VAR_TEXT // the text of the background variable as regexp

using namespace std;

DocTemplate::DocTemplate(const string &begin, const string &end) :
    begin_repr(begin), end_repr(end)
{
}

string
DocTemplate::output_begin(const string &title, const string &cs, const string &add,
    const string &header, const string &footer, const std::string &background,
    const string &input_lang)
{
    boost::regex title_exp(TITLE_VAR);
    boost::regex css_exp(CSS_VAR);
    boost::regex additional_exp(ADDITIONAL_VAR);
    boost::regex header_exp(HEADER_VAR);
    boost::regex footer_exp(FOOTER_VAR);
    boost::regex background_exp(BACKGROUND_VAR);
    boost::regex input_lang_exp(INPUT_LANG_VAR);

    string ret = subst(title_exp, begin_repr, title);
    ret = subst(css_exp, ret, cs);
    ret = subst(additional_exp, ret, add);
    ret = subst(header_exp, ret, header);
    ret = subst(footer_exp, ret, footer);
    ret = subst(background_exp, ret, background);
    ret = subst(input_lang_exp, ret, input_lang);

    return ret;
}

string
DocTemplate::output_end(const string &title, const string &cs, const string &add,
    const string &header, const string &footer, const std::string &background, 
    const string &input_lang)
{
    boost::regex title_exp(TITLE_VAR);
    boost::regex css_exp(CSS_VAR);
    boost::regex additional_exp(ADDITIONAL_VAR);
    boost::regex header_exp(HEADER_VAR);
    boost::regex footer_exp(FOOTER_VAR);
    boost::regex background_exp(BACKGROUND_VAR);
    boost::regex input_lang_exp(INPUT_LANG_VAR);

    string ret = subst(title_exp, end_repr, title);
    ret = subst(css_exp, ret, cs);
    ret = subst(additional_exp, ret, add);
    ret = subst(header_exp, ret, header);
    ret = subst(footer_exp, ret, footer);
    ret = subst(background_exp, ret, background);
    ret = subst(input_lang_exp, ret, input_lang);

    return ret;
}

}
