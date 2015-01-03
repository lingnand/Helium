//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "formattermanager.h"

namespace srchilite {

FormatterManager::FormatterManager(FormatterPtr _defaultFormatter) :
    defaultFormatter(_defaultFormatter) {
}

FormatterManager::~FormatterManager() {
}

FormatterPtr FormatterManager::getFormatter(const std::string &elem) const {
    // the formatter for this element is not present
    if (!formatterMap[elem].get()) {
        // use the default formatter and store it for future requests
        formatterMap[elem] = defaultFormatter;
    }

    return formatterMap[elem];
}

FormatterPtr FormatterManager::hasFormatter(const std::string &elem) const {
    FormatterMap::const_iterator formatter = formatterMap.find(elem);

    if (formatter == formatterMap.end())
        return FormatterPtr();

    return formatter->second;
}

void FormatterManager::addFormatter(const std::string &elem, FormatterPtr formatter) {
    formatterMap[elem] = formatter;
}

}
