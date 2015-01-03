//
// C++ Interface: NamedSubExpsLangElem
//
// Description: represents a regular expression made by many marked groups
// and each marked group represents a different language element
//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "namedsubexpslangelem.h"
#include "stringdef.h"
#include "tostringcollection.h"

using namespace std;

namespace srchilite {

NamedSubExpsLangElem::NamedSubExpsLangElem(const ElementNames *names, StringDef *def,
        bool ex, bool al) :
    StateStartLangElem("named subexps", ex, al), // "named subexps" is a bogus name
    elementNames(names), regexpDef(def)
{
}

NamedSubExpsLangElem::~NamedSubExpsLangElem() {
    if (elementNames)
        delete elementNames;
    if (regexpDef)
        delete regexpDef;
}

const std::string
NamedSubExpsLangElem::toString() const
{
  string res = StateStartLangElem::toString() + " " + collectionToString(elementNames, ',') +
      regexpDef->toString();
  return res;
}

const std::string
NamedSubExpsLangElem::toStringOriginal() const
{
  string res = StateStartLangElem::toString() + " " + collectionToString(elementNames, ',') +
  regexpDef->toStringOriginal();
  return res;
}

}
