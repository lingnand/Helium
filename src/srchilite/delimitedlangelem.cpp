//
// C++ Implementation: delimitedlangelem
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

#include "delimitedlangelem.h"

#include "stringdef.h"

using namespace std;

namespace srchilite {

DelimitedLangElem::DelimitedLangElem(const string &n, StringDef *s, StringDef *e, StringDef *es, bool multi, bool nes)
 : StateStartLangElem(n), start(s), end(e), escape(es), multiline(multi), nested(nes)
{
}


DelimitedLangElem::~DelimitedLangElem()
{
  if (start)
    delete start;
  if (end)
    delete end;
  if (escape)
    delete escape;
}

const std::string
DelimitedLangElem::toString() const
{
  string res = StateStartLangElem::toString() + " " + start->toString() + (end ? " " + end->toString() : "");
  return res;
}

const std::string
DelimitedLangElem::toStringOriginal() const
{
  string res = StateStartLangElem::toString() + " " + start->toStringOriginal() + (end ? " " + end->toStringOriginal() : "");
  return res;
}

}
