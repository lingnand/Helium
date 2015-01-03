//
// C++ Implementation: languageinfer
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

#include "languageinfer.h"
#include "fileutil.h"

#include <boost/regex.hpp>

using namespace std;

namespace srchilite {

LanguageInfer::LanguageInfer() {
}

LanguageInfer::~LanguageInfer() {
}

const string LanguageInfer::infer(const string &filename) {
    istream *stream = open_file_istream_or_error(filename);

    string result = infer(*stream);

    delete stream;

    return result;
}

const string LanguageInfer::infer(istream &stream) {
    // the regular expression for finding the language specification in a script file
    // this such as #! /bin/bash
    static boost::regex
            langRegEx(
                    "#[[:blank:]]*![[:blank:]]*(?:[\\./]*)(?:[[:alnum:]]+[\\./]+)*([[:alnum:]]+)");

    // the regular expression for finding the language specification in a script file
    // this such as #! /usr/bin/env perl
    static boost::regex
            langEnvRegEx(
                    "#[[:blank:]]*![[:blank:]]*(?:[\\./]*)(?:[[:alnum:]]+[\\./]+)*(?:env)[[:blank:]]+([[:alnum:]]+)");

    // the regular expression for finding the language specification in a script file
    // according to Emacs convention: # -*- language -*-
    static boost::regex
            langRegExEmacs("-\\*-[[:blank:]]*([[:alnum:]]+).*-\\*-");

    // the Emacs specification has the precedence in order to correctly infer
    // that scripts of the shape
    // #!/bin/sh
    // #  -*- tcl -*-
    // are Tcl scripts and not shell scripts

    // the regular expression for scripts starting with <?...
    // such as xml and php
    static boost::regex langXMLLikeScripts("<\\?([[:alnum:]]+)");

    // the regular expression for <!DOCTYPE
    static boost::regex langDocType("<![Dd][Oo][Cc][Tt][Yy][Pp][Ee]");

    string firstLine;
    string secondLine;

    // read only the first line of the input
    read_line(&stream, firstLine);
    // and the second line
    read_line(&stream, secondLine);

    boost::match_results<std::string::const_iterator> what;
    boost::match_results<std::string::const_iterator> whatEnv;
    boost::match_results<std::string::const_iterator> whatEmacs;

    // first try the emacs specification
    boost::regex_search(secondLine, whatEmacs, langRegExEmacs,
            boost::match_default);

    if (whatEmacs[1].matched)
        return whatEmacs[1];
    else {
        // try also on the first line
        boost::regex_search(firstLine, whatEmacs, langRegExEmacs,
                boost::match_default);
        if (whatEmacs[1].matched)
            return whatEmacs[1];
    }

    // try also the env specification
    boost::regex_search(firstLine, whatEnv, langEnvRegEx, boost::match_default);

    if (whatEnv[1].matched)
        return whatEnv[1];

    // try the sha-bang specification
    boost::regex_search(firstLine, what, langRegEx, boost::match_default);

    if (what[1].matched)
        return what[1];

    // the xml like starting scripts
    boost::regex_search(firstLine, what, langXMLLikeScripts,
            boost::match_default);

    if (what[1].matched)
        return what[1];

    // the doctype case
    boost::regex_search(firstLine, what, langDocType,
            boost::match_default);

    if (what[0].matched)
        return "xml";

    return "";
}

}
