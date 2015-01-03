//
// C++ Implementation: langelemsprinter
//
// Description:
//
//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "langelemsprinter.hpp"

#include "statelangelem.h"
#include "langelems.h"
#include "namedsubexpslangelem.h"

#include <ostream>
#include <algorithm>
#include <iterator>

using namespace std;

namespace srchilite {

LangElemsPrinter::LangElemsPrinter() {
}

LangElemsPrinter::~LangElemsPrinter() {
}

void LangElemsPrinter::print(const LangElems *elems, ostream &os) {
    if (!elems)
        return;

    collect_DB(elems);

    copy(setOfElements.begin(), setOfElements.end(), ostream_iterator<
            SetOfElements::value_type>(os, "\n"));
}

void LangElemsPrinter::collect(const LangElems *elems) {
    if (elems)
        for (LangElems::const_iterator it = elems->begin(); it != elems->end(); ++it)
            collect_DB(*it);
}

void LangElemsPrinter::collect(const StateLangElem *elem) {
    setOfElements.insert(elem->getName());

    const LangElems *elems = elem->getElems();
    if (elems)
        collect_DB(elems);
}

void LangElemsPrinter::collect(const LangElem *elem) {
    setOfElements.insert(elem->getName());
}

void LangElemsPrinter::collect(const NamedSubExpsLangElem *elem) {
    const ElementNames *names = elem->getElementNames();

    setOfElements.insert(names->begin(), names->end());
}

#include "langelemsprinter_dbtab.cc"

}
