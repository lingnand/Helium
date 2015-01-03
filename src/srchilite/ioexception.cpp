//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ioexception.h"

namespace srchilite {

IOException::IOException(const std::string &_message,
        const std::string &_filename) :
    message(_message + (_filename.size() ? " " + _filename : "")), filename(_filename) {
}

IOException::~IOException() throw() {
}

const char* IOException::what() const throw () {
    return message.c_str();
}

std::ostream& operator<<(std::ostream& os, const IOException &entry) {
    os << entry.message << ".";

    return os;
}

}
