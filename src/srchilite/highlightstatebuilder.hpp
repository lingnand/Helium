//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef HIGHLIGHTSTATEBUILDER_H_
#define HIGHLIGHTSTATEBUILDER_H_

#include "highlightstate.h"

namespace srchilite {

class LangElems;
class HighlightRuleFactory;

/**
 * Builds an HighlightState from the language definition file collected structures.
 * This class uses dynamic overloading and it must be processed by doublecpp,
 * http://doublecpp.sf.net, in case you need to modify it.
 */
// doublecpp: forward declarations, DO NOT MODIFY
class DelimitedLangElem; // file: delimitedlangelem.h
class HighlightState; // file: highlightstate.h
class LangElem; // file: langelem.h
class NamedSubExpsLangElem; // file: namedsubexpslangelem.h
class StateLangElem; // file: statelangelem.h
class StringListLangElem; // file: stringlistlangelem.h
// doublecpp: end, DO NOT MODIFY

#line 22 "highlightstatebuilder.h"
class HighlightStateBuilder
{
    /// the factory for creating rules
    HighlightRuleFactory *highlightRuleFactory;
public:
    HighlightStateBuilder(HighlightRuleFactory *_highlightRuleFactory);
    virtual ~HighlightStateBuilder();

    /**
     * Builds all the rules into the passed mainState as specified in the
     * passed LangElems
     *
     * @param elems the element definitions retrieved from the lang file
     * @param mainState the main state where to store all the rules
     */
    void build(LangElems *elems, HighlightStatePtr mainState);

    /// the following is a multi-method that needs to be processed by doublecpp
// doublecpp: method branches, DO NOT MODIFY
#line 40 "highlightstatebuilder.h"
virtual void build(LangElem * elem, HighlightState * state);
#line 41 "highlightstatebuilder.h"
virtual void build(StringListLangElem * elem, HighlightState * state);
#line 42 "highlightstatebuilder.h"
virtual void build(DelimitedLangElem * elem, HighlightState * state);
#line 43 "highlightstatebuilder.h"
virtual void build(NamedSubExpsLangElem * elem, HighlightState * state);
#line 44 "highlightstatebuilder.h"
virtual void build(StateLangElem * elem, HighlightState * state);
virtual void build_DB(LangElem * elem, HighlightState * state);
#line 44 "highlightstatebuilder.h"
// doublecpp: end, DO NOT MODIFY

};

}

#endif /*HIGHLIGHTSTATEBUILDER_H_*/
