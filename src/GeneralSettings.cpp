/*
 * GeneralSettings.cpp
 *
 *  Created on: May 25, 2015
 *      Author: lingnan
 */

#include <GeneralSettings.h>

GeneralSettings::GeneralSettings(int highlightRange, QObject *parent):
    QObject(parent),
    _highlightRange(highlightRange)
{}

void GeneralSettings::setHighlightRange(int range)
{
    if (range != _highlightRange) {
        _highlightRange = range;
        emit highlightRangeChanged(_highlightRange);
    }
}
