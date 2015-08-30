/*
 * qgitrefspec.cpp
 *
 *  Created on: Aug 30, 2015
 *      Author: lingnan
 */

#include "qgitrefspec.h"

namespace LibQGit2
{

Refspec::Refspec(const git_refspec *data)
    : d(data)
{
}

QString Refspec::source() const
{
    return QString(git_refspec_src(d));
}

QString Refspec::destination() const
{
    return QString(git_refspec_dst(d));
}

QString Refspec::toString()
{
    return QString(git_refspec_string(d));
}

Direction Refspec::direction() const
{
    return (Direction) git_refspec_direction(d);
}

bool Refspec::sourceMatches(const QString &refName) const
{
    return git_refspec_src_matches(d, refName.toLocal8Bit().constData());
}

bool Refspec::destinationMatches(const QString &refName) const
{
    return git_refspec_dst_matches(d, refName.toLocal8Bit().constData());
}

} /* namespace LibQGit2 */
