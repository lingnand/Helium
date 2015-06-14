/*
 * NumberPicker.cpp
 *
 *  Created on: Jun 14, 2015
 *      Author: lingnan
 */

#include <bb/cascades/StandardPickerItem>
#include <NumberPicker.h>
#include <Utility.h>

using namespace bb::cascades;

NumberPicker::NumberPicker(int lowerBoundary, int upperBoundary):
    _provider(lowerBoundary, upperBoundary)
{
    setPickerItemProvider(&_provider);
    conn(this, SIGNAL(selectedValueChanged(const QVariant&)),
        this, SLOT(onSelectedValueChanged(const QVariant&)));
}


void NumberPicker::setSelectedNumber(int number)
{
    select(0, number);
}

VisualNode *NumberPicker::Provider::createItem(Picker *pickerList, int columnIndex)
{
    return StandardPickerItem::create();
}

void NumberPicker::Provider::updateItem(Picker *pickerList,
        int columnIndex, int rowIndex, VisualNode *pickerItem)
{
    ((StandardPickerItem *) pickerItem)->setTitle(QString::number(rowIndex));
}

int NumberPicker::Provider::columnCount() const
{
    return 1;
}

void NumberPicker::Provider::range(int column, int* lowerBoundary, int* upperBoundary)
{
    *lowerBoundary = _lowerBoundary;
    *upperBoundary = _upperBoundary;
}

QVariant NumberPicker::Provider::value(Picker *picker,
        const QList<int> & indices) const
{
    return QVariant(indices[0]);
}

void NumberPicker::onSelectedValueChanged(const QVariant &v)
{
    bb::cascades::Picker::setDescription(v.toString());
    emit selectedNumberChanged(v.toInt());
}
