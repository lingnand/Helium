/*
 * qgitdiffoptions.h
 *
 *  Created on: Aug 12, 2015
 *      Author: lingnan
 */

#ifndef QGITDIFFOPTIONS_H_
#define QGITDIFFOPTIONS_H_

#include "git2.h"
#include <QSharedPointer>
#include "libqgit2_config.h"

namespace LibQGit2
{

class LIBQGIT2_EXPORT DiffOptions
{
public:
    enum Flag {
        /** Normal diff, the default */
        Normal = GIT_DIFF_NORMAL,

        /*
         * Options controlling which files will be in the diff
         */

        /** Reverse the sides of the diff */
        Reverse = GIT_DIFF_REVERSE,

        /** Include ignored files in the diff */
        IncludeIgnored = GIT_DIFF_INCLUDE_IGNORED,

        /** Even with GIT_DIFF_INCLUDE_IGNORED, an entire ignored directory
         *  will be marked with only a single entry in the diff; this flag
         *  adds all files under the directory as IGNORED entries, too.
         */
        RecurseIgnoredDirs = GIT_DIFF_RECURSE_IGNORED_DIRS,

        /** Include untracked files in the diff */
        IncludeUntracked = GIT_DIFF_INCLUDE_UNTRACKED,

        /** Even with GIT_DIFF_INCLUDE_UNTRACKED, an entire untracked
         *  directory will be marked with only a single entry in the diff
         *  (a la what core Git does in `git status`); this flag adds *all*
         *  files under untracked directories as UNTRACKED entries, too.
         */
        RecurseUntrackedDirs = GIT_DIFF_RECURSE_UNTRACKED_DIRS,

        /** Include unmodified files in the diff */
        IncludeUnmodified = GIT_DIFF_INCLUDE_UNMODIFIED,

        /** Normally, a type change between files will be converted into a
         *  DELETED record for the old and an ADDED record for the new; this
         *  options enabled the generation of TYPECHANGE delta records.
         */
        IncludeTypechange = GIT_DIFF_INCLUDE_TYPECHANGE,

        /** Even with GIT_DIFF_INCLUDE_TYPECHANGE, blob->tree changes still
         *  generally show as a DELETED blob.  This flag tries to correctly
         *  label blob->tree transitions as TYPECHANGE records with new_file's
         *  mode set to tree.  Note: the tree SHA will not be available.
         */
        IncludeTypechangeTrees = GIT_DIFF_INCLUDE_TYPECHANGE_TREES,

        /** Ignore file mode changes */
        IgnoreFilemode = GIT_DIFF_IGNORE_FILEMODE,

        /** Treat all submodules as unmodified */
        IgnoreSubmodules = GIT_DIFF_IGNORE_SUBMODULES,

        /** Use case insensitive filename comparisons */
        IgnoreCase = GIT_DIFF_IGNORE_CASE,

        /** If the pathspec is set in the diff options, this flags means to
         *  apply it as an exact match instead of as an fnmatch pattern.
         */
        DisablePathspecMatch = GIT_DIFF_DISABLE_PATHSPEC_MATCH,

        /** Disable updating of the `binary` flag in delta records.  This is
         *  useful when iterating over a diff if you don't need hunk and data
         *  callbacks and want to avoid having to load file completely.
         */
        SkipBinaryCheck = GIT_DIFF_SKIP_BINARY_CHECK,

        /** When diff finds an untracked directory, to match the behavior of
         *  core Git, it scans the contents for IGNORED and UNTRACKED files.
         *  If *all* contents are IGNORED, then the directory is IGNORED; if
         *  any contents are not IGNORED, then the directory is UNTRACKED.
         *  This is extra work that may not matter in many cases.  This flag
         *  turns off that scan and immediately labels an untracked directory
         *  as UNTRACKED (changing the behavior to not match core Git).
         */
        EnableFastUntrackedDirs = GIT_DIFF_ENABLE_FAST_UNTRACKED_DIRS,

        /** When diff finds a file in the working directory with stat
         * information different from the index, but the OID ends up being the
         * same, write the correct stat information into the index.  Note:
         * without this flag, diff will always leave the index untouched.
         */
        UpdateIndex = GIT_DIFF_UPDATE_INDEX,

        /** Include unreadable files in the diff */
        IncludeUnreadable = GIT_DIFF_INCLUDE_UNREADABLE,

        /** Include unreadable files in the diff */
        IncludeUnreadableAsUntracked = GIT_DIFF_INCLUDE_UNREADABLE_AS_UNTRACKED,

        /*
         * Options controlling how output will be generated
         */

        /** Treat all files as text, disabling binary attributes & detection */
        ForceText = GIT_DIFF_FORCE_TEXT,
        /** Treat all files as binary, disabling text diffs */
        ForceBinary = GIT_DIFF_FORCE_BINARY,

        /** Ignore all whitespace */
        IgnoreWhitespace = GIT_DIFF_IGNORE_WHITESPACE,
        /** Ignore changes in amount of whitespace */
        IgnoreWhitespaceChange = GIT_DIFF_IGNORE_WHITESPACE_CHANGE,
        /** Ignore whitespace at end of line */
        IgnoreWhitespaceEol = GIT_DIFF_IGNORE_WHITESPACE_EOL,

        /** When generating patch text, include the content of untracked
         *  files.  This automatically turns on GIT_DIFF_INCLUDE_UNTRACKED but
         *  it does not turn on GIT_DIFF_RECURSE_UNTRACKED_DIRS.  Add that
         *  flag if you want the content of every single UNTRACKED file.
         */
        ShowUntrackedContent = GIT_DIFF_SHOW_UNTRACKED_CONTENT,

        /** When generating output, include the names of unmodified files if
         *  they are included in the git_diff.  Normally these are skipped in
         *  the formats that list files (e.g. name-only, name-status, raw).
         *  Even with this, these will not be included in patch format.
         */
        ShowUnmodified = GIT_DIFF_SHOW_UNMODIFIED,

        /** Use the "patience diff" algorithm */
        Patience = GIT_DIFF_PATIENCE,
        /** Take extra time to find minimal diff */
        Minimal = GIT_DIFF_MINIMAL,

        /** Include the necessary deflate / delta information so that `git-apply`
         *  can apply given diff information to binary files.
         */
        ShowBinary = GIT_DIFF_SHOW_BINARY,
    };

    enum SubmoduleIgnore {
        SubmoduleIgnoreReset = GIT_SUBMODULE_IGNORE_RESET, /**< reset to on-disk value */
        SubmoduleIgnoreNone = GIT_SUBMODULE_IGNORE_NONE,  /**< any change or untracked == dirty */
        SubmoduleIgnoreUntracked = GIT_SUBMODULE_IGNORE_UNTRACKED,  /**< dirty if tracked files change */
        SubmoduleIgnoreDirty = GIT_SUBMODULE_IGNORE_DIRTY,  /**< only dirty if HEAD moved */
        SubmoduleIgnoreAll = GIT_SUBMODULE_IGNORE_ALL,  /**< never dirty */
        SubmoduleIgnoreDefault = GIT_SUBMODULE_IGNORE_DEFAULT
    };

    Q_DECLARE_FLAGS(Flags, Flag)

    DiffOptions(Flags flags = Normal);

    void setFlags(Flags);

    void setSubmoduleIgnore(SubmoduleIgnore);

    void setPaths(const QList<QString> &paths);

    const git_diff_options* data() const;

private:
    class Private;
    QSharedPointer<Private> d_ptr;
    Q_DECLARE_PRIVATE()
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DiffOptions::Flags)

} /* namespace LibQGit2 */

#endif /* QGITDIFFOPTIONS_H_ */
