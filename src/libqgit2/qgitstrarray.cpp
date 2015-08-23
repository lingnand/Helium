/*
 * qgitstrarray.cpp
 *
 *  Created on: Aug 23, 2015
 *      Author: lingnan
 */

#include "qgitstrarray.h"
#include "private/strarray.h"

namespace LibQGit2
{

StrArray::StrArray(): m_arr(new internal::StrArray) {}
StrArray::StrArray(const QList<QByteArray> &list): m_arr(new internal::StrArray(list)) {}
StrArray::StrArray(const QList<QString> &paths): m_arr(new internal::StrArray(paths)) {}

void StrArray::set(const QList<QByteArray> &list)
{
    m_arr->set(list);
}

void StrArray::set(const QList<QString> &paths)
{
    m_arr->set(paths);
}

size_t StrArray::count() const
{
    return m_arr->count();
}

const git_strarray* StrArray::data() const
{
    return &m_arr->data();
}

} /* namespace LibQGit2 */
