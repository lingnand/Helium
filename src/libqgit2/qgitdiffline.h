/*
 * qgitdiffline.h
 *
 *  Created on: Aug 12, 2015
 *      Author: lingnan
 */

#ifndef QGITDIFFLINE_H_
#define QGITDIFFLINE_H_

#include "git2.h"

#include "libqgit2_config.h"

namespace LibQGit2
{

class LIBQGIT2_EXPORT DiffLine
{
public:
    enum Type {
        Context = GIT_DIFF_LINE_CONTEXT,
        Addition = GIT_DIFF_LINE_ADDITION,
        Deletion = GIT_DIFF_LINE_DELETION,

        ContextEOFNL = GIT_DIFF_LINE_CONTEXT_EOFNL, /**< Both files have no LF at end */
        AddEOFNL = GIT_DIFF_LINE_ADD_EOFNL,     /**< Old has no LF at end, new does */
        DelEOFNL = GIT_DIFF_LINE_DEL_EOFNL,     /**< Old has LF at end, new does not */

        /* The following values will only be sent to a `git_diff_line_cb` when
         * the content of a diff is being formatted through `git_diff_print`.
         */
        FileHDR = GIT_DIFF_LINE_FILE_HDR,
        HunkHDR = GIT_DIFF_LINE_HUNK_HDR,
        Binary = GIT_DIFF_LINE_BINARY    /**< For "Binary files x and y differ" */
    };
    DiffLine(const git_diff_line *line);
    Type type() const;
    int oldLineNumber() const; /**< Line number in old file or -1 for added line */
    int newLineNumber() const; /**< Line number in new file or -1 for deleted line */
    int numLines() const; /**< Number of newline characters in content */
    QString content() const;
private:
    const git_diff_line *d;
};

} /* namespace LibQGit2 */

#endif /* QGITDIFFLINE_H_ */
