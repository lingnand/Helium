//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <exception>
#include <cctype>

#include "highlightstatebuilder.hpp"

#include "langelems.h"
#include "stringlistlangelem.h"
#include "delimitedlangelem.h"
#include "highlightbuilderexception.h"
#include "highlightrulefactory.h"
#include "highlightrule.h"
#include "stringdef.h"
#include "namedsubexpslangelem.h"
#include "regexpreprocessor.h"
#include "statelangelem.h"
#include <sstream>
#include <ctype.h>

namespace srchilite {

/**
 * Sets the exit level of the rule using the information contained in the passed elem
 * @param elem
 * @param rule
 * @param inc the possible additional incrementation of the exit level
 */
static void setExitLevel(const StateStartLangElem *elem, HighlightRule *rule,
        unsigned int inc = 0);

/**
 * A string is to matched as isolated (word boundaries) basically if it is an alphanumerical
 * string or a _.
 *
 * Notice that this should be called only on strings specified in double quotes,
 * since other expressions are intended as regular expressions and should not
 * be isolated.  This is checked in the code that calls this function.
 *
 * @param s
 * @return
 */
static bool is_to_isolate(const string &s);

HighlightStateBuilder::HighlightStateBuilder(
        HighlightRuleFactory *_highlightRuleFactory) :
    highlightRuleFactory(_highlightRuleFactory) {
}

HighlightStateBuilder::~HighlightStateBuilder() {
}

bool is_to_isolate(const string &s) {
    if (s.size()) {
        if ((isalnum(s[0]) || s[0] == '_') && (isalnum(s[s.size() - 1])
                || s[s.size() - 1] == '_'))
            return true;
    }

    return false;
}

void HighlightStateBuilder::build(LangElems *elems, HighlightStatePtr mainState) {
    if (!elems)
        return;

    for (LangElems::const_iterator it = elems->begin(); it != elems->end(); ++it) {
        try {
            build_DB(*it, mainState.get());
        } catch (boost::regex_error &e) {
            // catch all other exceptions
            throw HighlightBuilderException("problem in this expression: "
                    + (*it)->toStringOriginal(), *it, e);
        }
    }
}

void HighlightStateBuilder::build(LangElem *elem, HighlightState *state) {
    // no common operation for the moment
}

void HighlightStateBuilder::build(StringListLangElem *elem,
        HighlightState *state) {
    const string &name = elem->getName();

    StringDefs *alternatives = elem->getAlternatives();
    WordList wordList;

    bool doubleQuoted = false, nonDoubleQuoted = false, buildAsWordList = true;

    for (StringDefs::const_iterator it = alternatives->begin(); it
            != alternatives->end(); ++it) {
        const string &rep = (*it)->toString();

        // double quoted strings generate WordListRules, otherwise simple ListRules

        // we don't allow double quoted strings mixed with non double quoted
        if (((*it)->isDoubleQuoted() && nonDoubleQuoted)
                || (!(*it)->isDoubleQuoted() && doubleQuoted)) {
            throw HighlightBuilderException(
                    "cannot mix double quoted and non double quoted", elem);
        }

        doubleQuoted = (*it)->isDoubleQuoted();
        nonDoubleQuoted = !(*it)->isDoubleQuoted();

        wordList.push_back(rep);

        // now check whether we must build a word list rule (word boundary) or an
        // ordinary list; as soon as we find something that is not to be isolated
        // we set buildAsWordList as false
        if (buildAsWordList && (!doubleQuoted || !is_to_isolate(rep))) {
            buildAsWordList = false;
        }
    }

    HighlightRulePtr rule;

    if (buildAsWordList)
        rule = HighlightRulePtr(highlightRuleFactory->createWordListRule(name,
                wordList, elem->isCaseSensitive()));
    else
        rule = HighlightRulePtr(highlightRuleFactory->createListRule(name,
                wordList, elem->isCaseSensitive()));

    rule->setAdditionalInfo(elem->toStringParserInfo());

    state->addRule(rule);

    setExitLevel(elem, rule.get());
}

void HighlightStateBuilder::build(DelimitedLangElem *elem,
        HighlightState *state) {
    const string &name = elem->getName();

    StringDef *start = elem->getStart();
    StringDef *end = elem->getEnd();
    StringDef *escape = elem->getEscape();

    string start_string = (start ? start->toString() : "");
    string end_string = (end ? end->toString() : "");
    string escape_string = (escape ? escape->toString() : "");

    if (elem->isNested() && start_string == end_string) {
        // the two delimiters must be different for nested elements
        throw HighlightBuilderException(
                "delimiters must be different for nested elements", elem);
    }

    bool end_string_has_references = false;
    // check possible back reference markers and their correctness
    if (end && end->hasBackRef() && end_string.size()) {
        backreference_info ref_info = RegexPreProcessor::num_of_references(
                end_string);
        subexpressions_info info =
                RegexPreProcessor::num_of_marked_subexpressions(start_string,
                        true, true);

        // possible errors, e.g., unbalanced parenthesis
        if (info.errors.size()) {
            throw HighlightBuilderException(info.errors, elem);
        }

        // check that there are enough subexpressions as requested by the maximal
        // back reference number
        unsigned int max = ref_info.second;
        if (max > info.marked) {
            std::ostringstream error;
            error << max << " subexpressions requested, but only "
                    << info.marked << " found";
            throw HighlightBuilderException(error.str(), elem);
        }

        end_string_has_references = true;
    }

    HighlightRulePtr rule;

    // if this element starts a new state/environment, we must split it
    if (elem->getStateLangElem() || elem->isMultiline()
            || end_string_has_references) {
        rule = HighlightRulePtr(highlightRuleFactory->createMultiLineRule(name,
                start_string, end_string, escape_string, elem->isNested()));

        if (end_string_has_references) {
            // record that the state (and the rule representing the end)
            // need to have dynamic back references replaced
            rule->getNextState()->setNeedsReferenceReplacement();
            rule->getNextState()->getRuleList().front()->setNeedsReferenceReplacement();

            // and that the starting rule has sub expressions
            // (that will be used for replacing dynamic back references)
            rule->setHasSubexpressions();

            // if the element is nested, then the last rule is a sort of copy
            // of the first one, so we need to record that it has subexpressions too
            if (elem->isNested()) {
                rule->getNextState()->getRuleList().back()->setHasSubexpressions();
            }
        }
    } else {
        rule = HighlightRulePtr(highlightRuleFactory->createLineRule(name,
                start_string, end_string, escape_string, elem->isNested()));
    }

    rule->setAdditionalInfo(elem->toStringParserInfo());
    state->addRule(rule);

    if (rule->getNextState().get()) {
        // as for exit level, if the rule was split using states, we must set
        // the exit level of the first rule of the next state (i.e., the end expression) of the rule
        // this exit level must be incremented by one: 1 is for exiting the inner state
        // of the rule, and 1 for exiting the state this rule belongs to
        setExitLevel(elem, rule->getNextState()->getRuleList().front().get(), 1);

        // adjust the additional info of the exiting rule
        rule->getNextState()->getRuleList().front()->setAdditionalInfo(
                elem->toStringParserInfo());

        // since this is a delimited element, we must set the default element for
        // the inner state to the name of the element itself
        rule->getNextState()->setDefaultElement(name);
    } else {
        setExitLevel(elem, rule.get());
    }
}

void HighlightStateBuilder::build(NamedSubExpsLangElem *elem,
        HighlightState *state) {
    const ElementNames *elems = elem->getElementNames();
    const StringDef *regexp = elem->getRegexpDef();
    const string &regexp_string = regexp->toString();

    // first check that the number of marked subexpressions is the same of
    // the specified element names
    subexpressions_info sexps =
            RegexPreProcessor::num_of_marked_subexpressions(regexp_string);

    if (sexps.errors.size()) {
        throw HighlightBuilderException(sexps.errors, elem);
    }

    if (sexps.marked != elems->size()) {
        throw HighlightBuilderException(
                "number of marked subexpressions does not match number of elements",
                elem);
    }

    HighlightRulePtr rule = HighlightRulePtr(
            highlightRuleFactory->createCompoundRule(*elems, regexp_string));

    rule->setAdditionalInfo(elem->toStringParserInfo());
    state->addRule(rule);

    setExitLevel(elem, rule.get());
}

void HighlightStateBuilder::build(StateLangElem *elem, HighlightState *state) {
    StateStartLangElem *statestart = elem->getStateStart();

    if (!elem->isState() && dynamic_cast<NamedSubExpsLangElem *> (statestart)) {
        throw HighlightBuilderException(
                "cannot use this element for environments (only for states)",
                statestart);
    }

    /*
     First act on the element that defines this new State
     */
    build_DB(statestart, state);

    /*
     The last rule corresponds to the rule added for
     the element that defines this state, i.e., statestart
     */
    HighlightRulePtr last = state->getRuleList().back();

    /*
     We must make sure that this rule has a next state,
     since we will use it to populate it with the elements of
     this state.

     For instance, a StringListLangElem does not have a next state,
     so we must create one.
     */
    if (!last->getNextState().get()) {
        last->setNextState(HighlightStatePtr(new HighlightState));
    }

    /*
     Use the last rule next state to populate it with
     the elements of this State/Environment element
     */
    HighlightStatePtr inner = last->getNextState();

    /*
     If it's a State then the default formatter corresponds to NORMAL,
     otherwise (Environment) the default formatter is the same of
     the element itself.
     */
    if (elem->isState()) {
        inner->setDefaultElement("normal");
    } else {
        // at this point we already checked that the rule represents only one element
        inner->setDefaultElement(last->getElemList().front());
    }

    last->setAdditionalInfo(statestart->toStringParserInfo());

    LangElems *elems = elem->getElems();
    build(elems, inner);
}

void setExitLevel(const StateStartLangElem *elem, HighlightRule *rule,
        unsigned int inc) {
    if (elem->exitAll()) {
        rule->setExitLevel(-1); // exit all
    } else if (elem->getExit()) {
        rule->setExitLevel(elem->getExit() + inc);
    }
}

#include "highlightstatebuilder_dbtab.cc"

}
