//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "highlighttoken.h"

using namespace std;

namespace srchilite {

HighlightToken::HighlightToken(const HighlightRule *_rule) :
    prefixOnlySpaces(false), matchedSize(0), rule(_rule) {
}

HighlightToken::HighlightToken(const std::string &elem,
        const std::string &_matched, const std::string &_prefix,
        const HighlightRule *_rule) :
    prefix(_prefix), prefixOnlySpaces(false), matchedSize(_matched.size()),
            rule(_rule) {
    addMatched(elem, _matched);
}

HighlightToken::~HighlightToken() {
}

void HighlightToken::addMatched(const std::string &elem, const std::string &s) {
    matched.push_back(make_pair(elem, s));
    matchedSize += s.size();
}

void HighlightToken::clearMatched() {
    if (matched.size()) {
        matched.clear();
        matchedSize = 0;
    }
    if (matchedSubExps.size()) {
        matchedSubExps.clear();
    }
}

}
