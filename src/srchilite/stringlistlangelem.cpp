//
// C++ Implementation: stringlistlangelem
//
// Description:
//
//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "stringlistlangelem.h"

#include "stringdef.h"
#include "tostringcollection.h"

using namespace std;

namespace srchilite {

StringListLangElem::StringListLangElem(const string &n, StringDefs *al, bool nons)
 : StateStartLangElem(n), alternatives(al), nonsensitive(nons)
{
}


StringListLangElem::~StringListLangElem()
{
  delete alternatives;
}

const std::string
StringListLangElem::toString() const
{
  string res = StateStartLangElem::toString() + " " + toStringCollection<StringDefs>(alternatives);
  return res;
}

const std::string
StringListLangElem::toStringOriginal() const
{
  string res = StateStartLangElem::toString() + " " + toStringOriginalCollection<StringDefs>(alternatives);
  return res;
}

}
