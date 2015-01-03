//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "highlightstate.h"
#include "highlightrule.h"
#include "tostringcollection.h"
#include "highlighttoken.h"

namespace srchilite {

unsigned int HighlightState::global_id = 1;

HighlightState::HighlightState(const std::string &e) :
    id(global_id++), defaultElement(e), needsReferenceReplacement(false) {
}

HighlightState::HighlightState(const HighlightState &copy) :
    id(global_id++), defaultElement(copy.defaultElement), ruleList(copy.ruleList),
            needsReferenceReplacement(copy.needsReferenceReplacement) {
}

HighlightState::~HighlightState() {
}

void HighlightState::addRule(HighlightRulePtr rule) {
    ruleList.push_back(rule);
}

HighlightRulePtr HighlightState::replaceRule(RuleList::size_type index,
        HighlightRulePtr rule) {
    HighlightRulePtr old = ruleList[index];
    ruleList[index] = rule;
    return old;
}

bool HighlightState::betterThan(const HighlightToken &t1,
        const HighlightToken &t2) {
    return (t1.prefix.size() < t2.prefix.size() || (t1.prefix.size()
            == t2.prefix.size() && t1.matchedSize > t2.matchedSize));
}

bool HighlightState::findBestMatch(std::string::const_iterator start,
        std::string::const_iterator end, HighlightToken &token,
        const MatchingParameters &params) const {
    HighlightToken bestToken, tempToken;
    const HighlightRule *bestMatchingRule = 0;
    HighlightRule *currentRule = 0;
    bool first = true;

    for (RuleList::const_iterator it = ruleList.begin(); it != ruleList.end(); ++it) {
        currentRule = (*it).get();
        tempToken.clearMatched();

        if (currentRule->tryToMatch(start, end, tempToken, params)) {
            if (first || betterThan(tempToken, bestToken)) {
                // the first one, it's always the best
                if (first) {
                    first = false;
                }
                bestToken.copyFrom(tempToken);
                bestMatchingRule = currentRule;

                // if we matched something with no prefix (or only spaces)...
                if (tempToken.prefixOnlySpaces) {
                    // ...don't try any other rule
                    break;
                }
            }
        }
    }

    if (!first) {
        // we found the best matching rule
        token.copyFrom(bestToken);
        token.rule = bestMatchingRule;
        return true;
    }

    return false;
}

bool HighlightState::findBestMatch(const std::string &s, HighlightToken &token,
        const MatchingParameters &params) const {
    return findBestMatch(s.begin(), s.end(), token, params);
}

void HighlightState::replaceReferences(const ReplacementList &rep) {
    for (size_t i = 0; i < ruleList.size(); ++i) {
        if (ruleList[i]->getNeedsReferenceReplacement()) {
            // create a copy
            ruleList[i] = HighlightRulePtr(ruleList[i]->clone());
            ruleList[i]->replaceReferences(rep);
        }
    }
}

}
