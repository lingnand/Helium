//
// C++ Implementation: statestartlangelem
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

#include "statestartlangelem.h"

using std::string;

namespace srchilite {

StateStartLangElem::StateStartLangElem(const string &n, unsigned int exit, bool all) :
    LangElem(n), exit(exit), exit_all(all), statelangelem(0)
{
}

StateStartLangElem::~StateStartLangElem()
{
}

const std::string
StateStartLangElem::toString() const
{
  string res = LangElem::toString();
  return res;
}

}
