/******************************************************************************
 * This file is part of the libqgit2 library
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "qgitremote.h"
#include "qgitexception.h"
#include "private/remotecallbacks.h"
#include "private/strarray.h"

#include "git2.h"

namespace LibQGit2 {

struct Remote::Private : public internal::RemoteListener
{
    Private(Remote &parent, git_remote *remote, const Credentials &credentials) :
        m_data(remote, git_remote_free),
        m_parent(parent),
        m_callbacks(remote, this, credentials)
    {
    }

    int progress(int transferProgress)
    {
        emit m_parent.transferProgress(transferProgress);
        return 0;
    }

    QSharedPointer<git_remote> m_data;

private:
    Remote &m_parent;
    internal::RemoteCallbacks m_callbacks;
};


Remote::Remote(git_remote *remote, const Credentials &credentials, QObject *parent) :
    QObject(parent),
    d_ptr(new Private(*this, remote, credentials))
{
}

QString Remote::name() const
{
    return QString(git_remote_name(data()));
}

QString Remote::url() const
{
    return QString(git_remote_url(data()));
}

void Remote::setUrl(const QString &url)
{
    qGitThrow(git_remote_set_url(data(), url.toLatin1()));
}

void Remote::save()
{
    qGitThrow(git_remote_save(data()));
}

size_t Remote::refspecCount() const
{
    return git_remote_refspec_count(data());
}

Refspec Remote::refspec(size_t n) const
{
    return Refspec(git_remote_get_refspec(data(), n));
}

void Remote::push(const QString &branch, const Signature &signature, const QString &message)
{
    internal::StrArray refs;
    if (!branch.isEmpty()) {
        refs.set(QList<QString>() << QString("HEAD:refs/heads/%1").arg(branch));
    }

    git_push_options opts = GIT_PUSH_OPTIONS_INIT;
    qGitThrow(git_remote_push(data(), refs.constData(), &opts, signature.data(), message.isNull() ? NULL : message.toUtf8().constData()));
}

void Remote::fetch(const QString& head, const Signature &signature, const QString &message)
{
    internal::StrArray refs;
    if (!head.isEmpty()) {
        refs.set(QList<QString>() << QString("refs/heads/%2:refs/remotes/%1/%2").arg(name()).arg(head));
    }

    qGitThrow(git_remote_fetch(data(), refs.count() > 0 ? refs.constData() : NULL, signature.data(), message.isNull() ? NULL : message.toUtf8().constData()));
}

void Remote::prune()
{
    qGitThrow(git_remote_prune(data()));
}

QList<Remote::Head> Remote::list()
{
    qGitThrow(git_remote_connect(data(), GIT_DIRECTION_FETCH));
    qGitEnsureValue(1, git_remote_connected(data()));

    /* List the heads on the remote */
    const git_remote_head** remote_heads = NULL;
    size_t count = 0;
    qGitThrow(git_remote_ls(&remote_heads, &count, data()));
    QList<Remote::Head> heads;
    for (size_t i = 0; i < count; ++i) {
        const git_remote_head* head = remote_heads[i];
        if (head) {
            heads << (Head) { head->local,
                    OId(&head->oid), OId(&head->loid),
                    QString(head->name), QString(head->symref_target) };
        }
    }

    return heads;
}

git_remote* Remote::data() const
{
    return d_ptr->m_data.data();
}

}
