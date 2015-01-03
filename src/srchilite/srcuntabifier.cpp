//
// C++ Implementation: srcuntabifier
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

#include "srcuntabifier.h"
#include <sstream>
#include <boost/regex.hpp>

namespace srchilite {

const boost::regex tabexp("[\\t]");

const string Untabifier::doPreformat(const string &s) {
    boost::sregex_iterator m1(s.begin(), s.end(), tabexp);
    boost::sregex_iterator m2;

    if (m1 == m2) {
        if (s[0] == '\n')
            n_ = 0; // reset char counter
        else
            n_ += s.size();

        return s;
    }

    std::ostringstream buffer;
    string prefix;
    string suffix;

    for (boost::sregex_iterator it = m1; it != m2; ++it) {
        prefix = it->prefix();
        suffix = it->suffix();
        if (prefix.size()) {
            buffer << prefix;
            n_ += prefix.size();
        }

        int nSpaces = nSpacesPerTab_ - n_ % nSpacesPerTab_;
        buffer << string(nSpaces, ' ');
        n_ += nSpaces;
    }

    if (suffix.size()) {
        buffer << suffix;
        n_ += suffix.size();
    }

    return buffer.str();
}

}
