//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "highlightstateprinter.h"

#include "highlightstate.h"
#include "highlightrule.h"
#include "highlighttoken.h"
#include "tostringcollection.h"

namespace srchilite {

#define INDENT_INC 2
#define inc_indent (indent += INDENT_INC)
#define dec_indent (indent -= INDENT_INC)
#define do_indent stream << (indent ? string(indent, ' ') : "");

using namespace std;

HighlightStatePrinter::HighlightStatePrinter(std::ostream &s) :
    indent(0), stream(s) {
}

HighlightStatePrinter::~HighlightStatePrinter() {
}

void HighlightStatePrinter::printHighlightState(const HighlightState *state) {
    do_indent;
    stream << "STATE " << state->getId() << " default: "
            << state->getDefaultElement() << endl;

    const RuleList &ruleList = state->getRuleList();

    inc_indent;

    for (RuleList::const_iterator it = ruleList.begin(); it != ruleList.end(); ++it) {
        printHighlightRule((*it).get());
    }

    dec_indent;
}

void HighlightStatePrinter::printHighlightRule(const HighlightRule *rule) {
    HighlightStatePtr nextState = rule->getNextState();

    do_indent;

    stream << "rule (" + toStringCollection(rule->getElemList()) + ") \""
            << rule->toString() << "\" (exit level: " << rule->getExitLevel()
            << ", next: " << (nextState.get() ? nextState->getId() : 0)
            << (rule->isNested() ? ", nested" : "") << ")" << endl;

    // recursive call on next level
    if (nextState.get()) {
        unsigned int id = nextState->getId();

        // avoid printing already printed states and avoid loops for recursive states
        if (stateidset.find(id) == stateidset.end()) {
            stateidset.insert(id);
            inc_indent;
            printHighlightState(nextState.get());
            dec_indent;
        }
    }
}

void HighlightStatePrinter::printHighlightToken(const HighlightToken *token) {
    stream << "prefix : \"" << token->prefix << "\"" << endl;
    stream << "suffix : \"" << token->suffix << "\"" << endl;
    stream << "matched: " << endl;

    for (MatchedElements::const_iterator it = token->matched.begin(); it
            != token->matched.end(); ++it)
        stream << "    \"" << it->second << "\": " << it->first << endl;

    if (token->rule)
        stream << "rule   : " << token->rule->toString() << endl;
}

}
