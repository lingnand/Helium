//
// C++ Implementation: vardefinitions
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

#include "vardefinitions.h"
#include "stringdef.h"
#include "tostringcollection.h"

using namespace std;

namespace srchilite {

VarDefinitions::VarDefinitions()
{
}


VarDefinitions::~VarDefinitions()
{
}

void
VarDefinitions::add(const std::string &var, const StringDefs *values)
{
  if (contains(var))
    (*this)[var] += "|";

  (*this)[var] = toStringCollection<StringDefs>(values, '|');

  delete values;
}

const string &
VarDefinitions::getVar(const std::string &name)
{
  return (*this)[name];
}

bool
VarDefinitions::contains(const string &name)
{
  return find(name) != end();
}

}
