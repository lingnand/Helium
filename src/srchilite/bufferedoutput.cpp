//
// Author: Lorenzo Bettini <http://www.lorenzobettini.it>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "bufferedoutput.h"

namespace srchilite {

using namespace std;

BufferedOutput::BufferedOutput(std::ostream &os) :
    outputBuff(os), alwaysFlush(false) {
}

BufferedOutput::~BufferedOutput() {
}

void BufferedOutput::output(const std::string &s) {
    outputBuff << s;

    if (alwaysFlush)
        outputBuff << flush;
}

void BufferedOutput::postLineInsert(const std::string &s) {
    if (s.size())
        postLineContents.insert(s);
}

void BufferedOutput::postDocInsert(const std::string &s) {
    if (s.size())
        postDocContents.insert(s);
}

void BufferedOutput::writePostInfo(PostContents &post,
        const std::string &prefix) {
    for (PostContents::const_iterator it = post.begin(); it != post.end(); ++it) {
        output(prefix + *it);
    }

    post.clear();
}

void BufferedOutput::writePostLine(const std::string &prefix) {
    writePostInfo(postLineContents, prefix);
}

void BufferedOutput::writePostDoc(const std::string &prefix) {
    writePostInfo(postDocContents, prefix);
}

}
