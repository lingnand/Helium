/*
 * qgitpatch.cpp
 *
 *  Created on: Aug 12, 2015
 *      Author: lingnan
 */

#include <qgitpatch.h>

namespace LibQGit2
{

Patch::Patch(git_patch *patch):
    d(patch, git_patch_free)
{
}

DiffDelta Patch::delta() const
{
    const git_diff_delta *delta = 0;
    if (!d.isNull()) {
        delta = git_patch_get_delta(d.data());
    }
    return DiffDelta(delta);
}

size_t Patch::numHunks() const
{
    size_t ret = 0;
    if (!d.isNull()) {
        ret = git_patch_num_hunks(d.data());
    }
    return ret;
}

Patch::LineStats Patch::lineStats() const
{
    Patch::LineStats ls = {0, 0, 0};
    if (!d.isNull()) {
        git_patch_line_stats(&ls.totalContext, &ls.totalAdditions, &ls.totalDeletions,
                d.data());
    }
    return ls;
}

DiffHunk Patch::hunk(size_t index) const
{
    const git_diff_hunk *hunk = 0;
    size_t lines = 0;
    if (!d.isNull()) {
        git_patch_get_hunk(&hunk, &lines, d.data(), index);
    }
    return DiffHunk(d.data(), hunk, index, lines);
}

} /* namespace LibQGit2 */
