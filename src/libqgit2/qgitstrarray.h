/*
 * qgitstrarray.h
 *
 *  Created on: Aug 23, 2015
 *      Author: lingnan
 */

#ifndef QGITSTRARRAY_H_
#define QGITSTRARRAY_H_

#include "git2.h"
#include <QSharedPointer>
#include "libqgit2_config.h"

namespace LibQGit2
{

namespace internal {
    class StrArray;
}

class StrArray
{
public:
    explicit StrArray();
    explicit StrArray(const QList<QByteArray> &list);
    explicit StrArray(const QList<QString> &paths);
    void set(const QList<QByteArray> &list);

    void set(const QList<QString> &paths);

    size_t count() const;

    git_strarray* data() const;
    const git_strarray* constData() const;
private:
    QSharedPointer<internal::StrArray> m_arr;
};

} /* namespace LibQGit2 */

#endif /* QGITSTRARRAY_H_ */
