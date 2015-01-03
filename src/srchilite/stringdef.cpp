/*
 * Copyright (C) 1999-2007  Lorenzo Bettini <http://www.lorenzobettini.it>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <algorithm>

#include "stringdef.h"

namespace srchilite {

StringDef *StringDef::concat(const StringDef *s1, const StringDef *s2) {
    StringDef *ret = new StringDef(s1->stringdef + s2->stringdef);

    ret->setBackRef(s1->hasBackRef() || s2->hasBackRef());

    return ret;
}

const std::string StringDef::toString() const {
    return stringdef;
}

}
