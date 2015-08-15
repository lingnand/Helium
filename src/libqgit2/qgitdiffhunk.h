/*
 * qgitdiffhunk.h
 *
 *  Created on: Aug 12, 2015
 *      Author: lingnan
 */

#ifndef QGITDIFFHUNK_H_
#define QGITDIFFHUNK_H_

#include "libqgit2_config.h"

#include "qgitdiffline.h"

namespace LibQGit2
{

class LIBQGIT2_EXPORT DiffHunk
{
public:
    DiffHunk(git_patch *patch, const git_diff_hunk *hunk, size_t index, size_t numLines);
    size_t numLines() const;
    DiffLine line(size_t index) const;
    int oldStart() const;
    int oldLines() const;
    int newStart() const;
    int newLines() const;
    QString header() const;
private:
    const git_diff_hunk *d;
    git_patch *m_patch;
    size_t m_index;
    size_t m_numlines;
};

} /* namespace LibQGit2 */

#endif /* QGITDIFFHUNK_H_ */
