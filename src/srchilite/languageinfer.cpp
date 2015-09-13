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

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <vector>

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

const string guessEmacsMode(const string &modeline) {
    vector<string> strv;

    boost::split(strv, modeline, boost::is_any_of(";"));
    for (vector<string>::iterator it = strv.begin(); it != strv.end(); ++it) {
        boost::trim(*it);

        vector<string> tokens;
        boost::split(tokens, *it, boost::is_any_of(":"));
        boost::trim(tokens[0]);

        // A single token is considered a language definition
        if (tokens.size() == 1)
            return tokens[0];

        // otherwise, look for -*- mode: lang -*-
        boost::trim(tokens[1]);
        if (tokens[0] == "mode")
            return tokens[1];
    }

    return "";
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
            langRegExEmacs("-\\*-[[:blank:]]*([[:print:]]+).*-\\*-");

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

    if (whatEmacs[1].matched) {
        string guess = guessEmacsMode(whatEmacs[1]);
        if (guess != "")
            return guess;
    }

    // try also on the first line
    boost::regex_search(firstLine, whatEmacs, langRegExEmacs,
             boost::match_default);

    if (whatEmacs[1].matched) {
        string guess = guessEmacsMode(whatEmacs[1]);
        if (guess != "")
            return guess;
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
