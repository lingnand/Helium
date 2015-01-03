//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "parserexception.h"

#include "parsestruct.h"
#include <sstream>

namespace srchilite {

ParserException::ParserException(const std::string &_message,
        const ParseStruct *pinfo) :
    message(_message), filename((pinfo->path.size() ? pinfo->path + "/" : "")
            + pinfo->file_name), line(pinfo->line) {
}

ParserException::ParserException(const std::string &_message,
        const std::string &_filename, unsigned int _line) :
    message(_message), filename(_filename), line(_line) {
}

ParserException::~ParserException() throw() {
}

const char* ParserException::what() const throw () {
    return "error during the parsing of a definition file";
}

std::ostream& operator<<(std::ostream& os, const ParserException &entry) {
    std::ostringstream err;

    if (entry.filename.size())
        err << entry.filename << ":";

    if (entry.line > 0)
        err << entry.line << ": ";
    else if (entry.filename.size())
        err << " ";

    os << err.str() << entry.message;

    if (entry.additional.size())
        os << "\n" << err.str() << entry.additional;

    return os;
}

}
