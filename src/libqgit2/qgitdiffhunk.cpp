/*
 * qgitdiffhunk.cpp
 *
 *  Created on: Aug 12, 2015
 *      Author: lingnan
 */

#include "qgitdiffhunk.h"

namespace LibQGit2
{

DiffHunk::DiffHunk(git_patch *patch, const git_diff_hunk *hunk, size_t index, size_t numLines):
    d(hunk), m_patch(patch), m_index(index), m_numlines(numLines)
{
}

size_t DiffHunk::numLines() const
{
    return m_numlines;
}

DiffLine DiffHunk::line(size_t index) const
{
    const git_diff_line *line = 0;
    git_patch_get_line_in_hunk(&line, m_patch, m_index, index);
    return DiffLine(line);
}

int DiffHunk::oldStart() const
{
    return d->old_start;
}

int DiffHunk::oldLines() const
{
    return d->old_lines;
}

int DiffHunk::newStart() const
{
    return d->new_start;
}

int DiffHunk::newLines() const
{
    return d->new_lines;
}

QString DiffHunk::header() const
{
    return QString::fromAscii(d->header, d->header_len);
}

} /* namespace LibQGit2 */
