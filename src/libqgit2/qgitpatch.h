/*
 * qgitpatch.h
 *
 *  Created on: Aug 12, 2015
 *      Author: lingnan
 */

#ifndef QGITPATCH_H_
#define QGITPATCH_H_

#include "git2.h"
#include <QSharedPointer>
#include "libqgit2_config.h"

#include "qgitdiffdelta.h"
#include "qgitdiffhunk.h"

namespace LibQGit2
{

class LIBQGIT2_EXPORT Patch
{
public:
    Patch(git_patch *patch = 0);

    DiffDelta delta() const;

    size_t numHunks() const;

    struct LineStats {
        size_t totalContext;
        size_t totalAdditions;
        size_t totalDeletions;
    };
    LineStats lineStats() const;

    DiffHunk hunk(size_t index) const;
public:
    QSharedPointer<git_patch> d;
};

} /* namespace LibQGit2 */

#endif /* QGITPATCH_H_ */
