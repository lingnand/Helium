//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "langelem.h"
#include <sstream>

namespace srchilite {

const std::string LangElem::toStringParserInfo() const {
    std::ostringstream os;

    os << filename;

    if (line > 0)
        os << ":" << line;

    return os.str();
}

}
