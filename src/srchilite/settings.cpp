/*
 * settings.cpp
 *
 *  Created on: Apr 18, 2009
 *      Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2008
 *  Copyright: See COPYING file that comes with this distribution
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "settings.h"

#ifndef USE_MSVC
// msvc does not provide this header
#include <dirent.h>
#else
#include <io.h>
#include <direct.h>
#define mkdir(path,mode) _mkdir (path)
#include <compat_dirent.h>
#endif

#include <sys/stat.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <boost/regex.hpp>

#include "fileutil.h"
#include "verbosity.h"

#ifdef USE_MINGW
// mingw's mkdir() function has 1 argument, but we pass 2 arguments.
// so we use its version _mkdir()
#undef mkdir
#define mkdir(path,mode) _mkdir (path)
#endif

// define ABSOLUTEDATADIR
#define ABSOLUTEDATADIR "app/native/assets/srchilite"

using namespace std;

namespace srchilite {

/// if set (i.e., not empty), retrieveDataDir() always returns this value
string globalDataDir = "app/native/assets/srchilite";

/// the regular expression for the conf file syntax
boost::regex
        datadir_exp(
                "[[:blank:]]*(datadir)[[:blank:]]*=[[:blank:]]*\"([^[:blank:]\\r]+)\"[[:blank:]\\r]*|([[:space:]]+)|([[:space:]]*#.*)");

#define DEFAULT_CONF_DIR ".source-highlight"
#define DEFAULT_CONF_FILE "source-highlight.conf"

static string findHomeDirectory() {
    const char *home = getenv("HOME");
    if (home) {
        return home;
    }

    // let's try other variables in windows
    char * homedrive = getenv("HOMEDRIVE");	// Drive containing Windows
    char * homepath = getenv("HOMEPATH");	// Path to the user directory into the Windows drive
    if (homedrive && homepath) {
        return string(homedrive) + string(homepath);
    }

    // last chance in windows
    home = getenv("USERPROFILE");
    if (home) {
        return home;
    }

    // nothing found...
    return "";
}

Settings::Settings() :
    confFileName(DEFAULT_CONF_FILE),
    testFileName("lang.map"), dataDir(ABSOLUTEDATADIR) {
    const string homeDir = findHomeDirectory();

    confDir = (homeDir != "" ? homeDir + "/" + DEFAULT_CONF_DIR + "/" : string(
            DEFAULT_CONF_DIR) + "/");
}

Settings::~Settings() {

}

bool Settings::checkForTestFile() {
    string file = dataDir + "/" + testFileName;
    ifstream i(file.c_str());

    return (i != 0);
}

bool Settings::checkForConfFile() {
    string file = confDir + confFileName;
    ifstream i(file.c_str());

    return (i != 0);
}

bool Settings::readDataDir() {
    string file = confDir + confFileName;
    ifstream i(file.c_str());

    string line;
    if (i != 0) {
        while (read_line(&i, line)) {
            if (line.size()) {
                boost::cmatch what;
                if (boost::regex_match(line.c_str(), what, datadir_exp)
                        && what[2].matched) {// not all spaces, nor comments
                    dataDir = what[2];
                    return true;
                }
            }
        }
    }

    return false;
}

SettingError Settings::save() {
    // the directory does not exist?
    if (opendir(confDir.c_str()) == 0) {
        // then create it
        if (mkdir(confDir.c_str(), S_IRWXU) != 0) {
            return CANT_CREATE_DIR;
        }
    }

    string file = confDir + confFileName;
    ofstream o;
    o.open(file.c_str());

    if (o) {
        o << "# settings for source-highlight" << endl;
        o << endl;
        o << "datadir = \"" << dataDir << "\"" << endl;
        o.close();
        return NO_SETTING_ERROR;
    }

    return CANT_CREATE_FILE;
}

const string Settings::retrieveDataDir(bool reload) {
    if (globalDataDir != "")
        return globalDataDir;

    static string dataDir;

    if (dataDir != "" && !reload)
        return dataDir; // cached value

    VERBOSELN("retrieving default datadir value...");

    const char *_dataDir = getenv("SOURCE_HIGHLIGHT_DATADIR");
    if (_dataDir) {
        VERBOSELN("using SOURCE_HIGHLIGHT_DATADIR env value " + string(_dataDir));
        dataDir = _dataDir;
        return _dataDir;
    }

    static Settings settings;

    if (!settings.readDataDir()) {
        VERBOSELN("using hardcoded datadir value " ABSOLUTEDATADIR);
        dataDir = ABSOLUTEDATADIR;
        return ABSOLUTEDATADIR;
    }

    dataDir = settings.getDataDir();

    VERBOSELN("using datadir value from conf file " + dataDir);

    return dataDir;
}

const std::string Settings::getDefaultDataDir() {
    return ABSOLUTEDATADIR;
}

void Settings::setGlobalDataDir(const std::string &dataDir) {
    globalDataDir = dataDir;
}

bool Settings::checkSettings() {
    static Settings settings;

    settings.setDataDir(retrieveDataDir());
    return settings.checkForTestFile();
}

}
