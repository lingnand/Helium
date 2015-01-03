#include "stringlistlangelem.h"

#include "statelangelem.h"

#include "namedsubexpslangelem.h"

#include "delimitedlangelem.h"

#include "langelem.h"

#include "highlightstatebuilder.hpp"

void
HighlightStateBuilder::build_DB(LangElem * elem, HighlightState * state)
{
  elem->dispatch_build(this, state);
}

void
DelimitedLangElem::dispatch_build(HighlightStateBuilder *HighlightStateBuilder_o, HighlightState * state)
{
  HighlightStateBuilder_o->build(this, state);
}

void
LangElem::dispatch_build(HighlightStateBuilder *HighlightStateBuilder_o, HighlightState * state)
{
  HighlightStateBuilder_o->build(this, state);
}

void
NamedSubExpsLangElem::dispatch_build(HighlightStateBuilder *HighlightStateBuilder_o, HighlightState * state)
{
  HighlightStateBuilder_o->build(this, state);
}

void
StateLangElem::dispatch_build(HighlightStateBuilder *HighlightStateBuilder_o, HighlightState * state)
{
  HighlightStateBuilder_o->build(this, state);
}

void
StringListLangElem::dispatch_build(HighlightStateBuilder *HighlightStateBuilder_o, HighlightState * state)
{
  HighlightStateBuilder_o->build(this, state);
}

