//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "stringtable.h"

using namespace std;

namespace srchilite {

StringTable::StringTable() {
}

StringTable::~StringTable() {
    // delete all the store strings
    for (const_iterator it = begin(); it != end(); ++it)
        delete *it;
}

std::string *StringTable::newString(const std::string &s) {
    string *newstring = new string(s);
    push_back(newstring);
    return newstring;
}

}
