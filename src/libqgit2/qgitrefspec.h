/*
 * qgitrefspec.h
 *
 *  Created on: Aug 30, 2015
 *      Author: lingnan
 */

#ifndef QGITREFSPEC_H_
#define QGITREFSPEC_H_

#include "git2.h"

#include "libqgit2_config.h"
#include "qgitnet.h"

namespace LibQGit2
{

class LIBQGIT2_EXPORT Refspec
{
public:
    explicit Refspec(const git_refspec *data);
    QString source() const;
    QString destination() const;
    QString toString();
    Direction direction() const;
    bool sourceMatches(const QString &refName) const;
    bool destinationMatches(const QString &refName) const;
private:
    const git_refspec *d;
};

} /* namespace LibQGit2 */

#endif /* QGITREFSPEC_H_ */
