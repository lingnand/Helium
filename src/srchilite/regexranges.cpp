/*
 * regexranges.cpp
 *
 *  Created on: Apr 11, 2009
 *      Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2008
 *  Copyright: See COPYING file that comes with this distribution
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "regexranges.h"

namespace srchilite {

RegexRanges::RegexRanges() :
    currentRegex(0) {

}

RegexRanges::~RegexRanges() {

}

bool RegexRanges::addRegexRange(const std::string &s) {
    try {
        ranges.push_back(boost::regex(s));
    } catch (boost::regex_error &e) {
        return false;
    }

    return true;
}

const boost::regex *RegexRanges::matches(const std::string &line) {
    for (RegexRangesType::const_iterator it = ranges.begin(); it
            != ranges.end(); ++it) {
        if (boost::regex_search(line, *it)) {
            return &(*it);
        }
    }

    return 0;
}

bool RegexRanges::isInRange(const std::string &line) {
    // if the current regular expression is null, then we still
    // haven't found the starting of the range
    if (!currentRegex) {
        currentRegex = matches(line);

        // note that even if we found a matching regular expression,
        // the delimiters of the range must not be considered
        return false;
    } else {
        // we're already inside a range, and to find the end of it
        // we must match the current regex
        if (boost::regex_search(line, *currentRegex)) {
            // ok we found the end of the range
            // so first, reset current regular expression
            currentRegex = 0;
            return false;
        }
    }

    // if we're here, we're still in range
    return true;
}

}
