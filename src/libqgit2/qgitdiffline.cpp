/*
 * qgitdiffline.cpp
 *
 *  Created on: Aug 12, 2015
 *      Author: lingnan
 */

#include <qgitdiffline.h>

namespace LibQGit2
{

DiffLine::DiffLine(const git_diff_line *line):
    d(line)
{
}

DiffLine::Type DiffLine::type() const
{
    return (DiffLine::Type) d->origin;
}

int DiffLine::oldLineNumber() const
{
    return d->old_lineno;
}

int DiffLine::newLineNumber() const
{
    return d->new_lineno;
}

int DiffLine::numLines() const
{
    return d->num_lines;
}

QString DiffLine::content() const
{
    return QString::fromUtf8(d->content, d->content_len);
}

} /* namespace LibQGit2 */
