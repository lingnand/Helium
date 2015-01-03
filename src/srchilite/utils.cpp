//
// C++ Implementation: utils
//
// Description:
//
//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "utils.h"
#include <algorithm>
#include <cctype>      // old <ctype.h>

namespace srchilite {

struct ToLower
{
  char operator() (char c) const  { return std::tolower(c); }
};

struct ToUpper
{
  char operator() (char c) const  { return std::toupper(c); }
};

void Utils::toUpper(std::string &s)
{
  // Change everything into upper case
  std::transform (s.begin(), s.end(), s.begin(), ToUpper());
}

void Utils::toLower(std::string &s)
{
  // Change everything into upper case
  std::transform (s.begin(), s.end(), s.begin(), ToLower());
}

std::string Utils::toupper(const std::string &s)
{
  std::string buffer = s;
  toUpper(buffer);
  return buffer;
}

std::string Utils::tolower(const std::string &s)
{
  std::string buffer = s;
  toLower(buffer);
  return buffer;
}

}
