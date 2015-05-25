/*
 * HighlightRangePicker.cpp
 *
 *  Created on: May 25, 2015
 *      Author: lingnan
 */

#include <bb/cascades/StandardPickerItem>
#include <bb/cascades/Label>
#include <HighlightRangePicker.h>
#include <Utility.h>

using namespace bb::cascades;

#define HIGHLIGHT_RANGE_LOWER_BOUNDARY 0
#define HIGHLIGHT_RANGE_UPPER_BOUNDARY 40

HighlightRangePicker::HighlightRangePicker()
{
    setPickerItemProvider(&_provider);
    conn(this, SIGNAL(selectedValueChanged(const QVariant&)),
        this, SLOT(onSelectedValueChanged(const QVariant&)));
    select(0, 20);
    onTranslatorChanged();
}

VisualNode *HighlightRangePicker::Provider::createItem(Picker *pickerList, int columnIndex)
{
    return StandardPickerItem::create();
}

void HighlightRangePicker::Provider::updateItem(Picker *pickerList,
        int columnIndex, int rowIndex, VisualNode *pickerItem)
{
    ((StandardPickerItem *) pickerItem)->setTitle(QString::number(rowIndex));
}

int HighlightRangePicker::Provider::columnCount() const
{
    return 1;
}

void HighlightRangePicker::Provider::range(int column, int* lowerBoundary, int* upperBoundary)
{
    *lowerBoundary = HIGHLIGHT_RANGE_LOWER_BOUNDARY;
    *upperBoundary = HIGHLIGHT_RANGE_UPPER_BOUNDARY;
}

QVariant HighlightRangePicker::Provider::value(Picker *picker,
        const QList<int> & indices) const
{
    return QVariant(indices[0]);
}

void HighlightRangePicker::onSelectedValueChanged(const QVariant &v)
{
    bb::cascades::Picker::setDescription(v.toString());
    emit highlightRangeChanged(v.toInt());
}

void HighlightRangePicker::onTranslatorChanged()
{
    setTitle(tr("Highlight Range"));
}
