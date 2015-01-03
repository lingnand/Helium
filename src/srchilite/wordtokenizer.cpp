//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/regex.hpp>

#include "wordtokenizer.h"

namespace srchilite {

/**
 * the regular expression for detecting spaces and words
 */
static boost::regex string_or_space_regex("([^[:blank:]]+)|([[:blank:]]+)");

using namespace std;

#define SPACE 2
#define NOT_SPACE 1

void WordTokenizer::tokenize(const std::string &s,
        WordTokenizerResults &results) {
    boost::sregex_iterator i(s.begin(), s.end(), string_or_space_regex);
    boost::sregex_iterator j;
    while (i != j) {
        if ((*i)[SPACE].matched) {
            results.push_back(make_pair(string((*i)[SPACE].first, (*i)[SPACE].second), ""));
        } else {
            results.push_back(make_pair("", string((*i)[NOT_SPACE].first, (*i)[NOT_SPACE].second)));
        }

        ++i;
    }
}

}
