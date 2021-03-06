/******************************************************************************
 * This file is part of the libqgit2 library
 * Copyright (c) 2011 Laszlo Papp <djszapi@archlinux.us>
 * Copyright (C) 2013 Leonardo Giordani
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

#include "qgitref.h"

#include "qgitoid.h"
#include "qgitrepository.h"
#include "qgitexception.h"
#include "private/pathcodec.h"

namespace LibQGit2
{

Reference::Reference(git_reference *ref)
    : d(ref, git_reference_free)
{
}

Reference::Reference(const Reference& other)
    : d(other.d)
{
}

Reference::~Reference()
{
}

OId Reference::target() const
{
    return OId(git_reference_target(d.data()));
}

OId Reference::peelToTarget() const
{
    return OId(git_reference_target_peel(d.data()));
}

QString Reference::symbolicTarget() const
{
    return QString::fromUtf8(git_reference_symbolic_target(d.data()));
}

bool Reference::isDirect() const
{
    return git_reference_type(d.data()) == GIT_REF_OID;
}

bool Reference::isSymbolic() const
{
    return git_reference_type(d.data()) == GIT_REF_SYMBOLIC;
}

QString Reference::name() const
{
    return QString::fromUtf8(git_reference_name(d.data()));
}

QString Reference::branchName() const
{
    const char *name = NULL;
    qGitThrow(git_branch_name(&name, d.data()));
    return QString(name);
}

void Reference::branchDelete()
{
    qGitThrow(git_branch_delete(d.data()));
}

bool Reference::branchIsHead() const
{
    return qGitThrow(git_branch_is_head(d.data()));
}

Reference Reference::resolve() const
{
    git_reference *ref;
    qGitThrow(git_reference_resolve(&ref, d.data()));
    return Reference(ref);
}

Tree Reference::peelToTree() const
{
    git_object *obj;
    qGitThrow(git_reference_peel(&obj, d.data(), GIT_OBJ_TREE));
    return Tree((git_tree *) obj);
}

Object Reference::peelToObject() const
{
    git_object *obj;
    qGitThrow(git_reference_peel(&obj, d.data(), GIT_OBJ_ANY));
    return Object(obj);
}

Commit Reference::peelToCommit() const
{
    git_object *obj;
    qGitThrow(git_reference_peel(&obj, d.data(), GIT_OBJ_COMMIT));
    return Commit((git_commit *) obj);
}

Tag Reference::peelToTag() const
{
    git_object *obj;
    qGitThrow(git_reference_peel(&obj, d.data(), GIT_OBJ_TAG));
    return Tag((git_tag *) obj);
}

Blob Reference::peelToBlob() const
{
    git_object *obj;
    qGitThrow(git_reference_peel(&obj, d.data(), GIT_OBJ_BLOB));
    return Blob((git_blob *) obj);
}

Repository Reference::owner() const
{
    return Repository(git_reference_owner(d.data()));
}

void Reference::setSymbolicTarget(const QString& target, const Signature &signature, const QString &message)
{
    git_reference* rp;
    qGitThrow(git_reference_symbolic_set_target(&rp, data(), PathCodec::toLibGit2(target), signature.data(), message.toUtf8()));
    d = ptr_type(rp, git_reference_free);
}

void Reference::setTarget(const OId& oid, const Signature &signature, const QString &message)
{
    git_reference* rp;
    qGitThrow(git_reference_set_target(&rp, data(), oid.constData(), signature.data(), message.toUtf8()));
    d = ptr_type(rp, git_reference_free);
}

bool Reference::isNull() const
{
    return data() == 0;
}

git_reference* Reference::data() const
{
    return d.data();
}

const git_reference* Reference::constData() const
{
    return d.data();
}

} // namespace LibQGit2
