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

#include "strarray.h"
#include "pathcodec.h"
#include <QtCore/QByteArray>

namespace LibQGit2 {
namespace internal {

StrArray::StrArray()
{
    m_data.count = 0;
    m_data.strings = NULL;
}

StrArray::StrArray(const QList<QString> &strings)
{
    m_data.strings = NULL;
    set(strings);
}

StrArray::~StrArray()
{
    git_strarray_free(&m_data);
}

void StrArray::set(const QList<QString> &strings)
{
    m_data.count = strings.size();
    if (m_data.count == 0) {
        git_strarray_free(&m_data);
        return;
    }

    m_data.strings = (char **) realloc(m_data.strings, m_data.count * sizeof(char *));
    for (size_t i = 0; i < m_data.count; ++i) {
        QByteArray arr = strings[i].toLocal8Bit();
        m_data.strings[i] = (char *) malloc(arr.size()+1);
        strcpy(m_data.strings[i], arr.data());
    }
}

size_t StrArray::count() const
{
    return m_data.count;
}

git_strarray* StrArray::data()
{
    return &m_data;
}

const git_strarray* StrArray::constData() const
{
    return &m_data;
}

QList<QString> StrArray::toStringList() const
{
    QList<QString> list;
    for (size_t i = 0; i < m_data.count; i++) {
        list.append(QString::fromLocal8Bit(m_data.strings[i]));
    }
    return list;
}

}
}
