/*
 *
 *  Created on: Apr 4, 2015
 *      Author: lingnan
 */
#include <Replacement.h>

QDebug operator<<(QDebug dbg, const TextSelection &selection)
{
    dbg.nospace() << "TextSelection(" << selection.start << "," << selection.end << ")";
    return dbg.maybeSpace();
}

QDebug operator<<(QDebug dbg, const Replacement &replacement)
{
    dbg.nospace() << "Replacement(" << replacement.selection << ", " << replacement.replacement << ")";
    return dbg.maybeSpace();
}
