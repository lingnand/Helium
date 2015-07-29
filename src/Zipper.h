/*
 * Zipper.h
 *
 *  Created on: Jul 28, 2015
 *      Author: lingnan
 */

#ifndef ZIPPER_H_
#define ZIPPER_H_

class Project;
class View;

class ZipperSignalsSlots : public QObject
{
    Q_OBJECT
public:
    ZipperSignalsSlots(QObject *parent=NULL): QObject(parent) {}
Q_SIGNALS:
    void activeItemChanged(Project *changed, Project *old);
    void itemInserted(int i, Project *);
    void itemRemoved(int i, Project *);
    void activeItemChanged(View *changed, View *old);
    void itemInserted(int i, View *);
    void itemRemoved(int i, View *);
};

template <class T>
class Zipper : public ZipperSignalsSlots
{
public:
    Zipper(QObject *parent=NULL): ZipperSignalsSlots(parent), _active(NULL) {}
    int indexOf(const T &t, int from = 0) const { return _list.indexOf(t, 0); }
    T &operator[](int i) { return _list[i]; }
    const T &operator[](int i) const { return _list[i]; }
    const T &at(int i) const { return _list.at(i); }
    void insert(int i, const T &t) {
        _list.insert(i, t);
        emit itemInserted(i, t);
        if (_list.size() == 1)
            setActive(t);
    }
    int size() const { return _list.size(); }
    bool empty() const { return _list.empty(); }
    void append(const T &t) { insert(size(), t); }
    // remove a given element and return the focus
    void removeAt(int i) {
        T o = _list.at(i);
        _list.removeAt(i);
        if (o == _active) {
            setActive(_list.empty() ? NULL : _list[qMax(i-1, 0)]);
        }
        emit itemRemoved(i, o);
    }
    const T &active() const { return _active; }
    void setActive(const T &t) {
        if (t != _active) {
            T old = _active;
            _active = t;
            emit activeItemChanged(_active, old);
        }
    }
private:
    QList<T> _list;
    T _active;
};

#endif /* ZIPPER_H_ */
