/*
 * qgitdiffoptions.cpp
 *
 *  Created on: Aug 12, 2015
 *      Author: lingnan
 */

#include "qgitdiffoptions.h"
#include "private/pathcodec.h"
#include "private/strarray.h"

namespace LibQGit2
{

class DiffOptions::Private
{
public:
    Private(Flags flags)
    {
        git_diff_init_options(&native, GIT_DIFF_OPTIONS_VERSION);
        setFlags(flags);
    }

    void setFlags(Flags flags)
    {
        native.flags = flags;
    }

    void setSubmoduleIgnore(SubmoduleIgnore ignore)
    {
        native.ignore_submodules = (git_submodule_ignore_t) ignore;
    }

    void setPaths(const QList<QString> &paths)
    {
        QList<QByteArray> pathByteArrays;
        pathByteArrays.reserve(paths.size());
        foreach (const QString &path, paths) {
            pathByteArrays.append(PathCodec::toLibGit2(path));
        }
        m_paths.set(pathByteArrays);
        native.pathspec = m_paths.data();
    }

    git_diff_options native;
    internal::StrArray m_paths;
};

DiffOptions::DiffOptions(Flags flags)
    : d_ptr(new Private(flags))
{
}

void DiffOptions::setFlags(Flags flags)
{
    d_ptr->setFlags(flags);
}

void DiffOptions::setSubmoduleIgnore(SubmoduleIgnore ignore)
{
    d_ptr->setSubmoduleIgnore(ignore);
}

void DiffOptions::setPaths(const QList<QString> &paths)
{
    d_ptr->setPaths(paths);
}

const git_diff_options* DiffOptions::data() const
{
    return &d_ptr->native;
}

} /* namespace LibQGit2 */
