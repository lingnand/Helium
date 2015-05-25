/*
 * HighlightRangePicker.h
 *
 *  Created on: May 25, 2015
 *      Author: lingnan
 */

#ifndef HIGHLIGHTRANGEPICKER_H_
#define HIGHLIGHTRANGEPICKER_H_

#include <bb/cascades/Picker>
#include <bb/cascades/PickerProvider>

class HighlightRangePicker : public bb::cascades::Picker
{
    Q_OBJECT
public:
    HighlightRangePicker();
    virtual ~HighlightRangePicker() {}
    Q_SLOT void onTranslatorChanged();
Q_SIGNALS:
    void highlightRangeChanged(int);
private:
    class Provider : public bb::cascades::PickerProvider {
    public:
        bb::cascades::VisualNode *createItem(bb::cascades::Picker *pickerList,
                int columnIndex);
        void updateItem(bb::cascades::Picker *pickerList,
                int columnIndex, int rowIndex,
                bb::cascades::VisualNode *pickerItem);
        int columnCount() const;
        void range(int column, int* lowerBoundary, int* upperBoundary);
        QVariant value(bb::cascades::Picker *picker, const QList<int> &indices) const;
    } _provider;
    Q_SLOT void onSelectedValueChanged(const QVariant &);
};

#endif /* HIGHLIGHTRANGEPICKER_H_ */
