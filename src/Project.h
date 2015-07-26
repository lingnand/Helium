/*
 * Project.h
 *
 *  Created on: Jul 25, 2015
 *      Author: lingnan
 */

#ifndef PROJECT_H_
#define PROJECT_H_

#include <bb/cascades/Tab>

class View;
class Buffer;

class Project : public bb::cascades::Tab
{
    Q_OBJECT
public:
    Project(const QString &path);
    virtual ~Project();
    View *activeView() const { return _activeView; }
    int activeViewIndex() const { return indexOf(_activeView); }
    void setActiveViewIndex(int);
    const QList<View *> &views() const { return _views; }
    const QString &path() const { return _path; };
    void setPath(const QString &path);
    void unselect();
    void select();
    int indexOf(View *v) const { return _views.indexOf(v); }
    View *at(int i) const { return _views[i]; }
    int size() { return _views.size(); }
    void insertNewView(int index, Buffer *);
    void addNewViewAndSetActive();
    void removeAt(int);
    void removeView(View *view) { removeAt(indexOf(view)); }
    void cloneAt(int);
    void cloneView(View *view) { cloneAt(indexOf(view)); }
Q_SIGNALS:
    void pathChanged(const QString &);
    void viewInserted(int index, View *);
    void viewRemoved(View *);
    void activeViewChanged(View *, bool triggeredFromSidebar);
    void translatorChanged();
private:
    View *_activeView;
    QList<View *> _views;
    QString _path;

    Q_SLOT void onViewTriggered();
};

#endif /* PROJECT_H_ */
