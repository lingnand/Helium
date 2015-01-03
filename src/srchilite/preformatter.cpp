//
// C++ Implementation: preformatter
//
// Description:
//
//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "preformatter.h"

using namespace std;

namespace srchilite {

PreFormatter::PreFormatter(PreFormatterPtr f) :
    decorator(f)
{
}

PreFormatter::~PreFormatter()
{
}

void
PreFormatter::setPreFormatter(PreFormatterPtr f)
{
  if (decorator.get())
    decorator->setPreFormatter(f);
  else
    decorator = f;
}

const string
PreFormatter::preformat(const string &text)
{
  if (! text.size())
    return text;

  string preformat_text = text;
  PreFormatter *inner = this;
  while (inner) // start the chain of preformatting
  {
    preformat_text = inner->doPreformat(preformat_text);
    inner = inner->decorator.get();
  }

  return preformat_text;
}

const string
PreFormatter::doPreformat(const string &text)
{
  return text;
}

}
