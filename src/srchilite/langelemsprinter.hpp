//
// C++ Interface: langelemsprinter
//
// Description:
//
//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef LANGELEMSPRINTER_H
#define LANGELEMSPRINTER_H

#include <set>
#include <string>
#include <ostream>

namespace srchilite {

/**
 * Prints all the language elements.
 * his class uses dynamic overloading and it must be processed by doublecpp,
 * http://doublecpp.sf.net, in case you need to modify it.
*/
// doublecpp: forward declarations, DO NOT MODIFY
class LangElem; // file: langelem.h
class LangElems; // file: langelems.h
class NamedSubExpsLangElem; // file: namedsubexpslangelem.h
class StateLangElem; // file: statelangelem.h
// doublecpp: end, DO NOT MODIFY

#line 26 "langelemsprinter.h"
class LangElemsPrinter
{
    typedef std::set<std::string> SetOfElements;
    SetOfElements setOfElements;

public:
    LangElemsPrinter();

    virtual ~LangElemsPrinter();

    /**
     * Prints all the elements contained in the passed LangElems
     * to the specified ostream.
     * @param elems
     * @param os
     */
    void print(const LangElems *elems, std::ostream &os);

protected:
// doublecpp: method branches, DO NOT MODIFY
#line 46 "langelemsprinter.h"
virtual void collect(const StateLangElem * elem);
#line 47 "langelemsprinter.h"
virtual void collect(const LangElem * elem);
#line 48 "langelemsprinter.h"
virtual void collect(const LangElems * elem);
#line 49 "langelemsprinter.h"
virtual void collect(const NamedSubExpsLangElem * elem);
public:
void _forward_collect(const LangElem * elem)
{
  collect(elem);
};

void _forward_collect(const LangElems * elem)
{
  collect(elem);
};

void _forward_collect(const NamedSubExpsLangElem * elem)
{
  collect(elem);
};

void _forward_collect(const StateLangElem * elem)
{
  collect(elem);
};

protected:
virtual void collect_DB(const LangElem * elem);
virtual void collect_DB(const LangElems * elem);
#line 49 "langelemsprinter.h"
// doublecpp: end, DO NOT MODIFY

};

}

#endif
