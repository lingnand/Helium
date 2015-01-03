/*
 * lineranges.cpp
 *
 *  Created on: Sep 17, 2008
 *      Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2008
 *  Copyright: See COPYING file that comes with this distribution
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "lineranges.h"
#include <sstream>

#include <boost/regex.hpp>
#include <cstdlib>

namespace srchilite {

/// regular expression for a single line
static boost::regex singleNumber("\\s*([[:digit:]]+)\\s*");
/// regular expression for an interval
static boost::regex rangeExp("\\s*([[:digit:]]+)\\s*-\\s*([[:digit:]]+)\\s*");
/// regular expression for an interval with only the first element
static boost::regex rangeExp1("\\s*([[:digit:]]+)\\s*-\\s*");
/// regular expression for an interval with only the second element
static boost::regex rangeExp2("\\s*-\\s*([[:digit:]]+)\\s*");

using namespace std;

LineRanges::LineRanges(unsigned int context) :
    searchFromTheStart(true), contextLines(context) {

}

LineRanges::~LineRanges() {
}

RangeError LineRanges::addRange(const std::string &range) {
    boost::smatch match;
    if (boost::regex_match(range, match, singleNumber)) {
        lineRangeSet.insert(make_pair(strtol(match[1].str().c_str(), 0, 0), 0));
    } else if (boost::regex_match(range, match, rangeExp)) {
        lineRangeSet.insert(make_pair(strtol(match[1].str().c_str(), 0, 0),
                strtol(match[2].str().c_str(), 0, 0)));
    } else if (boost::regex_match(range, match, rangeExp1)) {
        lineRangeSet.insert(make_pair(strtol(match[1].str().c_str(), 0, 0), -1));
    } else if (boost::regex_match(range, match, rangeExp2)) {
        lineRangeSet.insert(make_pair(-1, strtol(match[1].str().c_str(), 0, 0)));
    } else {
        return INVALID_RANGE_NUMBER;
    }

    return NO_ERROR;
}

RangeResult LineRanges::isInRange(const RangeElemType e) {
    if (searchFromTheStart) {
        currentRange = lineRangeSet.begin();
        searchFromTheStart = false;
    }

    while (currentRange != lineRangeSet.end()) {
        if (currentRange->first < 0) {
            // first and second cannot be both < 0 (already checked during add)
            if (e <= currentRange->second) {
                return IN_RANGE;
            }
        } else if (currentRange->second < 0) {
            if (e >= currentRange->first) {
                return IN_RANGE;
            } else {
                if ((contextLines > 0) && (currentRange->first - e)
                        <= contextLines) {
                    return CONTEXT_RANGE;
                }

                // makes no sense checking further ranges
                return NOT_IN_RANGE;
            }
        } else if (currentRange->second == 0) {
            // check perfect match
            if (e == currentRange->first) {
                return IN_RANGE;
            } else if (e < currentRange->first) {
                if (contextLines > 0) {
                    if (((currentRange->first - e) <= contextLines)) {
                        return CONTEXT_RANGE;
                    }
                }

                // makes no sense checking further ranges
                return NOT_IN_RANGE;
            } else if (contextLines > 0 && ((e - currentRange->first)
                    <= contextLines)) {
                return CONTEXT_RANGE;
            }
        } else if (e >= currentRange->first && e <= currentRange->second) {
            return IN_RANGE;
        } else if (contextLines > 0 && (((e < currentRange->first)
                && ((currentRange->first - e) <= contextLines)) || ((e
                > currentRange->second) && ((e - currentRange->second)
                <= contextLines)))) {
            return CONTEXT_RANGE;
        } else if (e < currentRange->first) {
            // makes no sense checking further ranges
            return NOT_IN_RANGE;
        }

        // if we're here we try with another range in the set
        currentRange++;
    }

    return NOT_IN_RANGE;
}

}
