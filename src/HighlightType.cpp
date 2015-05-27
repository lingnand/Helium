/*
 * HighlightType.cpp
 *
 *  Created on: May 26, 2015
 *      Author: lingnan
 */

#include <HighlightType.h>
#include <Filetype.h>

HighlightType HighlightType::fromFiletype(Filetype *ft)
{
    return ft ? ft->highlightType() : HighlightType();
}
