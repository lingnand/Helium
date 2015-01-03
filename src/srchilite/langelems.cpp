//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "langelems.h"

#include <sstream>
#include "tostringcollection.h"

using namespace std;

namespace srchilite {

LangElems::LangElems()
{
}


LangElems::~LangElems()
{
  for (LangElemsBase::iterator it = begin(); it != end(); ++it)
    delete *it;
}

const string
LangElems::toString() const
{
  return toStringCollection<LangElems>(this, '\n');
}

const string
LangElems::toStringOriginal() const
{
  return toStringOriginalCollection<LangElems>(this, '\n');
}

void LangElems::add(LangElem *el)
{
  push_back(el);

  // store the pointer to the element just inserted in the list
  elem_map[el->getName()].push_back((++rbegin()).base());
}

void LangElems::redef(LangElem *el)
{
  const string &name = el->getName();
  for (PointerList::iterator it = elem_map[name].begin(); it != elem_map[name].end(); ++it) {
    erase(*it); // remove previous elements with this name
  }
  elem_map[name].clear();
  add(el);
}

void LangElems::subst(LangElem *el)
{
  // replace the first occurrence with this lang element
  // remove the other occurrences if any
  const string &name = el->getName();
  Pointer first = end();
  for (PointerList::iterator it = elem_map[name].begin(); it != elem_map[name].end(); ++it) {
    if (first == end()) {
      **it = el;
      first = *it;
    } else {
      erase(*it); // remove remaining elements with this name
    }
  }
  elem_map[name].clear();
  elem_map[name].push_back(first);
}

}
