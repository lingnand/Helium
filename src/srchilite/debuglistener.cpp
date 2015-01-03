//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "debuglistener.h"
#include "highlightevent.h"
#include "highlighttoken.h"
#include "highlightrule.h"

using namespace std;

namespace srchilite {

DebugListener::DebugListener(std::ostream &_os) :
    os(_os), interactive(false) {
}

DebugListener::~DebugListener() {
}

//> TEXINFO
void DebugListener::notify(const HighlightEvent &event) {
    switch (event.type) {
    case HighlightEvent::FORMAT:
        // print information about the rule
        if (event.token.rule) {
            os << event.token.rule->getAdditionalInfo() << endl;
            os << "expression: \"" << event.token.rule->toString() << "\""
                    << endl;
        }

        // now format the matched strings
        for (MatchedElements::const_iterator it = event.token.matched.begin(); it
                != event.token.matched.end(); ++it) {
            os << "formatting \"" << it->second << "\" as " << it->first
                    << endl;
        }
        step();
        break;
    case HighlightEvent::FORMATDEFAULT:
        os << "formatting \"" << event.token.matched.front().second
                << "\" as default" << endl;
        step();
        break;
    case HighlightEvent::ENTERSTATE:
        os << "entering state: " << event.token.rule->getNextState()->getId()
                << endl;
        break;
    case HighlightEvent::EXITSTATE:
        int level = event.token.rule->getExitLevel();
        os << "exiting state, level: ";
        if (level < 0)
            os << "all";
        else
            os << level;
        os << endl;
        break;
    }
}
//> TEXINFO

void DebugListener::step() {
    if (!interactive) {
        return;
    }

    // this is only a quick solution to perform interaction
    string cmd;
    getline(cin, cmd);
}

}
