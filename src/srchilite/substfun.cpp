/**
 * C++ function: substfun.h
 *
 * Description: substitutes a string to a $var into a text.
 *
 * Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2005
 * Copyright: See COPYING file that comes with this distribution
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "substfun.h"

using namespace std;

namespace srchilite {

string
subst(const boost::regex &e, const string &s, const string &sub)
{
    string ret;

    boost::sregex_iterator i1(s.begin(), s.end(), e);
    boost::sregex_iterator i2;
    string suffix;

    if (i1 == i2)
        return s;
    // the exp is not in the string so we do not alter it.

    for (boost::sregex_iterator it = i1; it != i2; ++it) {
        string prefix = it->prefix();
        if (prefix.size())
            ret += prefix;

        suffix = it->suffix();
        ret += sub;
    }

    if (suffix.size())
        ret += suffix;

    return ret;
}

}
