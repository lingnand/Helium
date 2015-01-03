//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "highlightbuilderexception.h"

#include "parserinfo.h"
#include <sstream>

using namespace std;

namespace srchilite {

HighlightBuilderException::HighlightBuilderException(
        const std::string &_message, const ParserInfo *parserinfo,
        const boost::regex_error &e) :
    message(_message), causedBy(e), filename(parserinfo->filename),
            line(parserinfo->line) {
}

HighlightBuilderException::HighlightBuilderException(
        const std::string &_message, const std::string &_filename,
        unsigned int _line, const boost::regex_error &e) :
    message(_message), causedBy(e), filename(_filename), line(_line) {
}

HighlightBuilderException::~HighlightBuilderException() throw() {
}

const char* HighlightBuilderException::what() const throw () {
    return "problem when building an highlight rule";
}

std::ostream& operator<<(std::ostream& os,
        const HighlightBuilderException &entry) {
    ostringstream errbuff;

    if (entry.filename.size())
        errbuff << entry.filename << ":";

    if (entry.line > 0)
        errbuff << entry.line << ": ";
    else if (entry.filename.size())
        errbuff << " ";

    os << errbuff.str() << entry.message << "\n";

    const string &causeBy = entry.causedBy.what();
    if (causeBy.size()) {
        os << errbuff.str() << "Caused by: " << causeBy;
        /*
        if (entry.causedBy.position())
            os << "\n" << errbuff.str() << "Position: "
                    << entry.causedBy.position();
                    */
    }

    return os;
}

}
