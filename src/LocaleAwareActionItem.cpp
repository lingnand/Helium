/*
 * LocaleAwareActionItem.cpp
 *
 *  Created on: Aug 31, 2015
 *      Author: lingnan
 */

#include <LocaleAwareActionItem.h>
#include <Helium.h>

void LocaleAwareActionItem::onTranslatorChanged()
{
    setTitle(Helium::translate("Man", _title));
}
