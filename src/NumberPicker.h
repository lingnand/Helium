/*
 * NumberPicker.h
 *
 *  Created on: Jun 14, 2015
 *      Author: lingnan
 */

#ifndef NUMBERPICKER_H_
#define NUMBERPICKER_H_

#include <bb/cascades/Picker>
#include <bb/cascades/PickerProvider>

class NumberPicker : public bb::cascades::Picker
{
    Q_OBJECT
public:
    NumberPicker(int lowerBoundary, int upperBoundary);
    virtual ~NumberPicker() {}
    Q_SLOT void setSelectedNumber(int);
Q_SIGNALS:
    void selectedNumberChanged(int);
private:
    class Provider : public bb::cascades::PickerProvider {
    public:
        Provider(int lowerBoundary, int upperBoundary):
            _lowerBoundary(lowerBoundary), _upperBoundary(upperBoundary) {}
        bb::cascades::VisualNode *createItem(bb::cascades::Picker *pickerList,
                int columnIndex);
        void updateItem(bb::cascades::Picker *pickerList,
                int columnIndex, int rowIndex,
                bb::cascades::VisualNode *pickerItem);
        int columnCount() const;
        void range(int column, int* lowerBoundary, int* upperBoundary);
        QVariant value(bb::cascades::Picker *picker, const QList<int> &indices) const;
    private:
        int _lowerBoundary;
        int _upperBoundary;
    } _provider;
    Q_SLOT void onSelectedValueChanged(const QVariant &);
};

#endif /* NUMBERPICKER_H_ */
