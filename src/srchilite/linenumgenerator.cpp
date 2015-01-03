//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iomanip>

#include "linenumgenerator.h"
#include <sstream>

using namespace std;

namespace srchilite {

/// used for line information generation
static SubstitutionMapping substitutionmapping;

LineNumGenerator::LineNumGenerator(const TextStyle &_lineStyle,
        unsigned int _digitNum, char _padding) :
    digitNum(_digitNum), padding(_padding), lineStyle(_lineStyle) {
}

LineNumGenerator::~LineNumGenerator() {
}

const std::string LineNumGenerator::generateLine(unsigned int line) {
    ostringstream sout;

    sout << std::setw(digitNum) << std::setfill(padding) << line << ":";

    string line_str = lineStyle.output(sout.str());

    // possible anchor generation
    if (anchorStyle.toString() != "") {
        ostringstream line_n;
        line_n << anchorLinePrefix << line;
        substitutionmapping["$text"] = line_str;
        substitutionmapping["$linenum"] = line_n.str();
        line_str = anchorStyle.output(substitutionmapping);
    }

    return line_str + " ";
}

}
