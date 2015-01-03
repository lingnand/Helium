//
// C++ Implementation: langmap
//
// Description:
//
//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <boost/regex.hpp>

#include "langmap.h"
#include "fileutil.h"
#include "utils.h"
#include "parserexception.h"
#include "settings.h"

using namespace std;

namespace srchilite {

/// the regular expression for the map file syntax
boost::regex
        assoc_exp(
                "[[:blank:]]*([^[:blank:]]+)[[:blank:]]*=[[:blank:]]*([^[:blank:]\\r]+)[[:blank:]\\r]*|([[:space:]]+)|([[:space:]]*#.*)");

LangMap::LangMap(const string &_path, const string &_filename) :
    isOpen(false), path(_path), filename(_filename) {
}

LangMap::LangMap(const string &_filename) :
    isOpen(false), path(Settings::retrieveDataDir()), filename(_filename) {
}

LangMap::~LangMap() {
}

void LangMap::open() {
    if (isOpen)
        return;

    istream *in = open_data_file_istream(path, filename);
    string line;
    unsigned int lineno = 1;

    while (read_line(in, line)) {
        if (line.size()) {
            boost::cmatch what;
            if (!boost::regex_match(line.c_str(), what, assoc_exp))
                throw ParserException("wrong specification" + line, filename,
                        lineno);
            else if (!what[3].matched) // not all spaces
                langmap[what[1]] = what[2];
        }
        ++lineno;
    }

    isOpen = true;

    delete in;
}

const std::string LangMap::getMappedFileName(const std::string &lang) {
    // make sure that the lang map file has been parsed
    open();

    return getFileName(lang);
}

const std::string LangMap::getMappedFileNameFromFileName(
        const std::string &fileName) {
    // make sure the lang map file is parsed
    open();

    string mappedFile;

    // try with the file extension
    const string ext = get_file_extension(fileName);
    if (ext != "") {
        mappedFile = getFileName(ext);
        if (mappedFile != "")
            return mappedFile;
        else {
            mappedFile = getFileName(Utils::tolower(ext));
            if (mappedFile != "")
                return mappedFile;
        }
    }

    string fileNameNoPath = strip_file_path(fileName);

    // try with the file name
    mappedFile = getFileName(fileNameNoPath);
    if (mappedFile != "")
        return mappedFile;

    // try with the lower case file name (as our last chance)
    return getFileName(Utils::tolower(fileNameNoPath));
}

void LangMap::print() {
    for (Map::const_iterator it = langmap.begin(); it != langmap.end(); ++it)
        cout << it->first << " = " << it->second << endl;
}

set<string> LangMap::getLangNames() const {
    set<string> s;
    for (Map::const_iterator it = langmap.begin(); it != langmap.end(); ++it)
        if (it->first != "")
            s.insert(it->first);
    return s;
}

std::set<std::string> LangMap::getMappedFileNames() const {
    set<string> s;
    for (Map::const_iterator it = langmap.begin(); it != langmap.end(); ++it)
        if (it->second != "")
            s.insert(it->second);
    return s;

}

void LangMap::reload(const string &_path, const string &_filename) {
    path = _path;
    filename = _filename;
    isOpen = false; // this will force reopening
    langmap.clear();
    open();
}

}

