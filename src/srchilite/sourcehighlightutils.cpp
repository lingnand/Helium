/*
 * sourcehighlightutils.cpp
 *
 *  Created on: May 19, 2009
 *      Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2008
 *  Copyright: See COPYING file that comes with this distribution
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sourcehighlightutils.h"
#include "ioexception.h"
#include "fileutil.h"
#include "settings.h"

#include <stdio.h>

#ifndef USE_MSVC
// msvc does not provide this header
#include <dirent.h>
#else
#include <direct.h>
#define mkdir(path,mode) _mkdir (path)
#include <compat_dirent.h>
#endif

using namespace std;

namespace srchilite {

StringSet SourceHighlightUtils::getFileNames(const std::string path,
        const std::string fileExtension) {
    StringSet strings;

    DIR *dp;
    struct dirent *ep;

    dp = opendir(path.c_str());
    if (dp != NULL) {
        while ((ep = readdir(dp))) {
            const string name(ep->d_name);
            if (get_file_extension(name) == fileExtension) {
                strings.insert(name);
            }
        }
        (void) closedir(dp);
    } else {
        throw IOException("Couldn't open the directory", path);
    }

    return strings;
}

StringSet SourceHighlightUtils::getStyleFileNames(const std::string _path) {
    string path = _path;
    if (path == "")
        path = Settings::retrieveDataDir();

    return getFileNames(path, "style");
}

StringSet SourceHighlightUtils::getCssStyleFileNames(const std::string _path) {
    string path = _path;
    if (path == "")
        path = Settings::retrieveDataDir();

    return getFileNames(path, "css");
}

StringSet SourceHighlightUtils::getLangFileNames(const std::string _path) {
    string path = _path;
    if (path == "")
        path = Settings::retrieveDataDir();

    return getFileNames(path, "lang");
}

StringSet SourceHighlightUtils::getOutLangFileNames(const std::string _path) {
    string path = _path;
    if (path == "")
        path = Settings::retrieveDataDir();

    return getFileNames(path, "outlang");
}

}
