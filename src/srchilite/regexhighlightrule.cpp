//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "regexhighlightrule.h"
#include "highlighttoken.h"
#include "matchingparameters.h"
#include "regexpreprocessor.h"

namespace srchilite {

/// the only spaces regular expression
static boost::regex onlySpaces("[[:blank:]]*");

RegexHighlightRule::RegexHighlightRule(const std::string &s) :
    regExp(s) {

}

RegexHighlightRule::RegexHighlightRule(const std::string &name,
        const std::string &s) :
    HighlightRule(name), regExp(s) {

}

RegexHighlightRule::~RegexHighlightRule() {
}

bool RegexHighlightRule::tryToMatch(std::string::const_iterator start,
        std::string::const_iterator end, HighlightToken &token,
        const MatchingParameters &params) {
    boost::smatch match;
    boost::match_flag_type flags = boost::match_default;

    // whether this rule represents multiple elements
    bool hasMultipleElements = (getElemList().size() > 1);

    if (!params.beginningOfLine) {
        // the start of the string must not be interpreted as the beginning of the line
        flags |= boost::match_not_bol;
    }

    if (boost::regex_search(start, end, match, regExp, flags)) {
        token.prefix = match.prefix();
        token.suffix = match.suffix();

        // check that the prefix is empty or contains only spaces
        token.prefixOnlySpaces = boost::regex_match(token.prefix, onlySpaces);

        if (getHasSubexpressions()) {
            // record all the matched subexpressions
            for (unsigned int i = 1; i < match.size(); ++i) {
                token.matchedSubExps.push_back(match[i]);
            }
        }

        if (hasMultipleElements) {
            for (unsigned int i = 1; i < match.size(); ++i) {
                if (match[i].matched) {
                    token.addMatched(getElemList()[i - 1], match[i]);
                }
            }
        } else {
            token.addMatched(getElemList()[0], match[0]);
        }

        return true;
    }

    return false;
}

const std::string RegexHighlightRule::toString() const {
    return regExp.str();
}

void RegexHighlightRule::replaceReferences(const ReplacementList &rep) {
    regExp.assign(RegexPreProcessor::replace_references(regExp.str(), rep));
}

HighlightRule *RegexHighlightRule::clone() {
    return new RegexHighlightRule(*this);
}

void RegexHighlightRule::setRegExp(const std::string &s) {
    regExp.assign(s);
}

}
