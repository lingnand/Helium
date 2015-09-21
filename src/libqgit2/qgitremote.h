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

#ifndef LIBQGIT2_REMOTE_H
#define LIBQGIT2_REMOTE_H

#include "libqgit2_config.h"
#include "qgitsignature.h"
#include "qgitcredentials.h"
#include "qgitoid.h"
#include "qgitrefspec.h"

#include <QtCore/QSharedPointer>

struct git_remote;

namespace LibQGit2 {

/**
 * @brief Represents a git remote
 *
 * @ingroup LibQGit2
 * @{
 */
class LIBQGIT2_EXPORT Remote : public QObject
{
    Q_OBJECT

public:

    /**
     * @param remote The raw remote pointer. This needs to be initialized beforehand.
     *        This object takes ownership of this pointer and frees it when this object is destructed.
     * @param credentials Credentials to be used with this remote if any.
     * @param parent The parent of this QObject.
     */
    explicit Remote(git_remote *remote, const Credentials &credentials = Credentials(), QObject *parent = 0);

    QString name() const;

    /**
     * Gets the URL specified for this remote.
     */
    QString url() const;

    // call save() afterwards
    void setUrl(const QString &);

    void save();

    size_t refspecCount() const;

    Refspec refspec(size_t n) const;

    /**
     * Pushes HEAD to branch on this remote.
     * @param branch The local branch name to push to remote
     * @param signature The identity to use when updating reflogs.
     * @param message The message to insert into the reflogs. If left as the
     *        default (a null string), a message "update by push" is used.
     * @throws LibQGit2::Exception
     */
    void push(const QString &branch, const Signature &signature = Signature(), const QString &message = QString());

    /**
    * Fetch from remote.
    *
    * @param head Name of head to fetch (e.g. "master"). If left as the default
    *        the fetch heads configured for the remote are used.
    * @param signature The identity to use when updating reflogs
    * @param message The message to insert into the reflogs. If left as the
    *        default (a null string), a message "fetch <name>" is used , where <name>
    *        is the name of the remote (or its url, for in-memory remotes).
    * @throws LibQGit2::Exception
    */
    void fetch(const QString &head = QString(), const Signature &signature = Signature(), const QString &message = QString());

    void prune();

    struct Head {
        bool availableLocally;
        OId id, localId;
        QString name, symbolicTarget;
    };
    QList<Head> list();

    git_remote* data() const;

signals:
    void transferProgress(int);

private:
    Q_DISABLE_COPY(Remote)

    struct Private;
    QSharedPointer<Private> d_ptr;
};

/** @} */

}

Q_DECLARE_METATYPE(LibQGit2::Remote *)

#endif // LIBQGIT2_REMOTE_H
