//
// C++ Implementation: stopwatch
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

#include "stopwatch.h"

#include <iostream>

using namespace std;

namespace srchilite {

StopWatch::~StopWatch()
{
  clock_t total = clock() - start;
  cout << "elapsed time (secs): " << double(total) / CLOCKS_PER_SEC << endl;
}

}
