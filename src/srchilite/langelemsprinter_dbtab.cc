#include "statelangelem.h"

#include "namedsubexpslangelem.h"

#include "langelems.h"

#include "langelem.h"

#include "langelemsprinter.hpp"

void
LangElemsPrinter::collect_DB(const LangElem * elem)
{
  (const_cast<LangElem *>(elem))->dispatch_collect_const(this);
}

void
LangElemsPrinter::collect_DB(const LangElems * elem)
{
  (const_cast<LangElems *>(elem))->dispatch_collect_const(this);
}

void
LangElem::dispatch_collect_const(LangElemsPrinter *LangElemsPrinter_o)
{
  LangElemsPrinter_o->_forward_collect((const LangElem *)(this));
}

void
LangElems::dispatch_collect_const(LangElemsPrinter *LangElemsPrinter_o)
{
  LangElemsPrinter_o->_forward_collect((const LangElems *)(this));
}

void
NamedSubExpsLangElem::dispatch_collect_const(LangElemsPrinter *LangElemsPrinter_o)
{
  LangElemsPrinter_o->_forward_collect((const NamedSubExpsLangElem *)(this));
}

void
StateLangElem::dispatch_collect_const(LangElemsPrinter *LangElemsPrinter_o)
{
  LangElemsPrinter_o->_forward_collect((const StateLangElem *)(this));
}

