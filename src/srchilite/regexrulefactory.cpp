//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "regexrulefactory.h"
#include "regexhighlightrule.h"
#include "tostringcollection.h"
#include "highlightstate.h"
#include "regexpreprocessor.h"

namespace srchilite {

/**
 * Build a non-marking group (i.e., (? ... ) starting from s
 * @param s
 * @return
 */
static const std::string non_marking_group(const std::string &s) {
    return "(?:" + s + ")";
}

/**
 * Whether the passed string consists of a single char (escluding
 * the escape char back slash)
 * @param s
 * @return
 */
static bool is_single_char(const std::string &s) {
    if (s.size() && s[0] == '\\')
        return (s.size() == 2);

    return (s.size() == 1);
}

RegexRuleFactory::RegexRuleFactory() {
}

RegexRuleFactory::~RegexRuleFactory() {
}

HighlightRule *RegexRuleFactory::createSimpleRule(const std::string &name,
        const std::string &s) {
    return new RegexHighlightRule(name, s);
}

HighlightRule *RegexRuleFactory::createWordListRule(const std::string &name,
        const WordList &list, bool caseSensitive) {
    // \< and \> are word boundaries

    // we must make a non-marking group since the string can contain
    // alternative symbols. For instance,
    // \<(?:class|for|else)\>
    // correctly detects 'for' only in isolation, while
    // (?:\<class|for|else\>)
    // will not

    std::string alternatives = toStringCollection(list, '|');

    if (!caseSensitive)
        alternatives = RegexPreProcessor::make_nonsensitive(alternatives);

    return new RegexHighlightRule(name, "\\<" + non_marking_group(alternatives) + "\\>");
}

HighlightRule *RegexRuleFactory::createListRule(const std::string &name,
        const WordList &list, bool caseSensitive) {
    std::string alternatives = toStringCollection(list, '|');

    if (!caseSensitive)
        alternatives = RegexPreProcessor::make_nonsensitive(alternatives);

    return new RegexHighlightRule(name, non_marking_group(alternatives));
}

HighlightRule *RegexRuleFactory::createLineRule(const std::string &name,
        const std::string &start, const std::string &end,
        const std::string &escape, bool nested) {

    // if start or end is not a single char we cannot create a regexp directly:
    // we need to use states, and this is achieved by switching to a multi line rule
    if (nested || !is_single_char(start) || !is_single_char(end)) {
        return createMultiLineRule(name, start, end, escape, nested);
    }

    std::string exp_string;

    /*
     in case the expression is not the start element of a
     State/Environment and it must not spawn multiple lines, and the
     delimiters are only one character, build a regular
     expression of the shape

     <startdelim>(everything but delimiters)<enddelim>

     For instance if delimiters are "<" and ">" the built regular expression is

     "<(?:[^<>])*>"
     */
    if (!escape.size()) {
        exp_string = start + non_marking_group("[^" + start
                + (end != start ? end : "") + "]") + "*" + end;
    } else {
        /*
         in case of a specified escape character it will use it for the
         (everything but delimiters) part.
         For instace, if in the example above the escape character is the
         backslash, the generated expression is

         <(?:[^\\<\\>]|\\.)*>
         */
        exp_string = start + non_marking_group("[^" + escape + start + (end
                != start ? escape + end : "") + "]|"+ escape + ".") + "*" + end;
    }

    return createSimpleRule(name, exp_string);
}

HighlightRule *RegexRuleFactory::createMultiLineRule(const std::string &name,
        const std::string &start, const std::string &_end,
        const std::string &escape, bool nested) {

    std::string end = _end;
    if (!end.size()) {
        // if end is not specified, then end of buffer is assumed
        end = "\\z";
    }

    // rule matching the start expression
    HighlightRule *startRule = createSimpleRule(name, start);

    // rule matching the end expression
    HighlightRule *endRule = createSimpleRule(name, end);
    // when we match the end we must exit one level
    endRule->setExitLevel(1);

    // the state to enter when we match the start expression
    HighlightStatePtr innerState = HighlightStatePtr(new HighlightState);

    // this is the rule for exiting the inner state
    innerState->addRule(HighlightRulePtr(endRule));

    // if escape is given, we must not match the end string when preceeded by the
    // escape string, thus we build an expression that matches the escape sequence
    // followed by one character
    if (escape.size()) {
        HighlightRule *escapeRule = createSimpleRule(name, escape + ".");
        innerState->addRule(HighlightRulePtr(escapeRule));
    }

    if (nested) {
        // if the rule is nested it means that the start expression will
        // enter the same inner state.
        // rule matching the start expression within the inner state
        HighlightRule *nestedStartRule = createSimpleRule(name, start);
        nestedStartRule->setNested(true);
        innerState->addRule(HighlightRulePtr(nestedStartRule));
    }

    startRule->setNextState(innerState);

    return startRule;
}

HighlightRule *RegexRuleFactory::createCompoundRule(
        const ElemNameList &nameList, const std::string &rep) {
    HighlightRule *rule = new RegexHighlightRule(rep);

    for (ElemNameList::const_iterator it = nameList.begin(); it
            != nameList.end(); ++it) {
        rule->addElem(*it);
    }

    return rule;
}

}
