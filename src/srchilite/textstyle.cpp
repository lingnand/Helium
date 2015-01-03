/**
 * C++ Implementation: textstyle
 *
 * Description:
 *
 * Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2005
 * Copyright: See COPYING file that comes with this distribution
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "textstyle.h"
#include <sstream>

using namespace std;

namespace srchilite {

static SubstitutionMapping substitutionmapping;

TextStyle::TextStyle(const string &s,const char **vars) : repr((s.size() ? s : TEXT_VAR_TEXT)),
  invalid(true)
{
  ostringstream exp_buff;

  exp_buff << "\\$(style|text";

  if (vars) {
    int i = 0;
    while (vars[i]) {
      exp_buff << "|" << vars[i];
      ++i;
    }
  }

  exp_buff << ")";

  var_exp = boost::regex(exp_buff.str());
}

TextStyle::~TextStyle()
{
}

/**
 * The parts vector contains the string repr split in parts: those that
 * constant parts and those that represent $text and $style variables.
 * For instance, if repr == "<span class=\"$style\">$text</span>", then we
 * have:
 *
 *   - parts[0] == "<span class=\""
 *   - parts[1] == ""
 *   - parts[2] == "\">"
 *   - parts[3] == ""
 *   - parts[4] == "</span>"
 *
 * Then style_substitutions contains the index(es) of parts corresponding to
 * $style variable occurrences that will be substituted with values
 * and text_substitutions is the same but for $text occurrences.  Thus, in this
 * case:
 *
 * - style_substitutions[0] = 1
 * - text_substitutions[0] = 3
 */
void
TextStyle::build_vectors()
{
  parts.clear();
  substitutions.clear();

  boost::sregex_iterator i1(repr.begin(), repr.end(), var_exp);
  boost::sregex_iterator i2;
  string suffix;

  if (i1 == i2)
    return ;

  int index = -1;
  for (boost::sregex_iterator it = i1; it != i2; ++it) {
    string prefix = it->prefix();
    if (prefix.size()) {
      parts.push_back(prefix);
      ++index;
    }

    ++index;
    string matched = string((*it)[0].first, (*it)[0].second);
    substitutions[matched].push_back(index);

    suffix = it->suffix();
    parts.push_back(""); // this will be set during "output"
    // with a value for $text or $style
  }

  if (suffix.size())
    parts.push_back(suffix);
}

/**
 * First substitute in the vector parts the values for $text and
 * $style and return a string with the substituted values.
 *
 * @param text
 * @param style
 * @return
 */
string
TextStyle::output(const string &text, const string &style)
{
  substitutionmapping["$text"] = text;
  substitutionmapping["$style"] = style;

  return output(substitutionmapping);
}

/**
  * for each i substitutes: subst_map[i].first occurrence with subst_map[i].second
  */
std::string TextStyle::output(SubstitutionMapping &subst_map)
{
  if (invalid) {
    build_vectors();
    invalid = false;
  }

  SubstitutionIndexes::const_iterator subIt;
  IndexVector::const_iterator indexIt;

  for (subIt = substitutions.begin(); subIt != substitutions.end(); ++subIt) {
    for (indexIt = subIt->second.begin(); indexIt != subIt->second.end(); ++indexIt) {
      parts[*indexIt] = subst_map[subIt->first];
    }
  }

  string ret;

  for (StringVector::const_iterator it = parts.begin(); it != parts.end(); ++it)
    ret += *it;

  return ret;
}

string
TextStyle::subst_style(const string &style)
{
  return output(TEXT_VAR_TEXT, style); // leave $text untouched
}

TextStyle
TextStyle::compose(const TextStyle &inner)
{
  return TextStyle(output(inner.toString()));
}

void
TextStyle::update(const string &inner)
{
  repr = output(inner);
  invalid = true;
}

void
TextStyle::update(const string &text, const string &style)
{
  repr = output(text, style);
  invalid = true;
}

void
TextStyle::update(const TextStyle &inner)
{
	update(inner.toString());
}

bool
TextStyle::containsStyleVar() const
{
    boost::regex style_exp(STYLE_VAR);
    return boost::regex_search(repr, style_exp);
}

bool
TextStyle::empty() const
{
  return repr == STYLE_VAR_TEXT || repr == TEXT_VAR_TEXT;
}

}
