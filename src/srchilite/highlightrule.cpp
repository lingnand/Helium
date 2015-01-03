//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "highlightrule.h"

namespace srchilite {

HighlightRule::HighlightRule() :
    nextState(HighlightStatePtr()), exitLevel(0), nested(false),
            needsReferenceReplacement(false), hasSubexpressions(false) {

}

HighlightRule::HighlightRule(const std::string &name) :
    nextState(HighlightStatePtr()), exitLevel(0), nested(false),
            needsReferenceReplacement(false), hasSubexpressions(false) {
    elemList.push_back(name);
}

HighlightRule::~HighlightRule() {
}

void HighlightRule::addElem(const std::string &name) {
    elemList.push_back(name);
}

bool HighlightRule::tryToMatch(const std::string &s, HighlightToken &token,
        const MatchingParameters &params) {
    return tryToMatch(s.begin(), s.end(), token, params);
}

}
