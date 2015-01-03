/*
 * instances.cpp
 *
 *  Created on: Aug 21, 2009
 *      Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2008
 *  Copyright: See COPYING file that comes with this distribution
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "instances.h"

#include "regexrulefactory.h"
#include "langdefmanager.h"
#include "langmap.h"
#include "settings.h"

namespace srchilite {

static LangDefManager *langDefManager = 0;
static LangMap *langMap = 0;
static LangMap *outlangMap = 0;

LangDefManager *Instances::getLangDefManager() {
    if (!langDefManager)
        langDefManager = new LangDefManager(new RegexRuleFactory);

    return langDefManager;
}

LangMap *Instances::getLangMap() {
    if (!langMap)
        langMap = new LangMap(Settings::retrieveDataDir(), "lang.map");

    return langMap;
}

LangMap *Instances::getOutLangMap() {
    if (!outlangMap)
        outlangMap = new LangMap(Settings::retrieveDataDir(), "outlang.map");

    return outlangMap;
}

void Instances::reload() {
    if (!langMap)
        langMap = new LangMap(Settings::retrieveDataDir(), "lang.map");
    else
        langMap->reload(Settings::retrieveDataDir(), "lang.map");

    if (!outlangMap)
        outlangMap = new LangMap(Settings::retrieveDataDir(), "outlang.map");
    else
        outlangMap->reload(Settings::retrieveDataDir(), "outlang.map");
}

void Instances::unload() {
    if (langDefManager) {
        delete langDefManager->getRuleFactory();
        delete langDefManager;
        langDefManager = 0;
    }

    if (langMap) {
        delete langMap;
        langMap = 0;
    }

    if (outlangMap) {
        delete outlangMap;
        outlangMap = 0;
    }
}

}
