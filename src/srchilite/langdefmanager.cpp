//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "langdefmanager.h"

#include "fileutil.h"
#include "langdefparserfun.h"
#include "langelems.h"
#include "highlightstatebuilder.hpp"
#include "settings.h"

using namespace std;

namespace srchilite {

LangDefManager::LangDefManager(HighlightRuleFactory *_ruleFactory) :
    ruleFactory(_ruleFactory) {
}

LangDefManager::~LangDefManager() {
}

HighlightStatePtr LangDefManager::buildHighlightState(const string &path,
        const string &file) {
    HighlightStatePtr highlightState(new HighlightState);

    // parse the contents of the langdef file
    LangElems *elems = getLangElems(path, file);

    HighlightStateBuilder builder(ruleFactory);

    // build the highlight state corresponding to the language definition file
    builder.build(elems, highlightState);

    delete elems;

    return highlightState;
}

HighlightStatePtr LangDefManager::getHighlightState(const string &path,
        const string &file) {
    const string key = (path.size() ? path + "/" : "") + file;

    HighlightStatePtr highlightState = highlightStateCache[key];

    // check whether we had already built such an HighlightState
    if (highlightState.get())
        return highlightState;

    // otherwise build it
    highlightState = buildHighlightState(path, file);

    // store in the cache
    highlightStateCache[key] = highlightState;

    return highlightState;
}

HighlightStatePtr LangDefManager::getHighlightState(const string &file) {
    return getHighlightState(Settings::retrieveDataDir(), file);
}

LangElems *LangDefManager::getLangElems(const std::string &path,
        const std::string &file) {
    return parse_lang_def(path.c_str(), file.c_str());
}

}
