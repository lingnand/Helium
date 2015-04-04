/*
 * Type.cpp
 *
 *  Created on: Apr 4, 2015
 *      Author: lingnan
 */
#include <src/Type.h>

QDebug operator<<(QDebug dbg, const TextSelection &selection)
{
    return dbg.nospace() << "TextSelection(" << selection.start << "," << selection.end << ")";
}

QDebug operator<<(QDebug dbg, const Replacement &replacement)
{
    return dbg.nospace() << "Replacement(" << replacement.selection << ", " << replacement.replacement << ")";
}
