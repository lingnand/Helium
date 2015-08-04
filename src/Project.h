/*
 * Project.h
 *
 *  Created on: Jul 25, 2015
 *      Author: lingnan
 */

#ifndef PROJECT_H_
#define PROJECT_H_

#include <bb/cascades/Tab>
#include <Zipper.h>

class View;
class Buffer;

class Project : public bb::cascades::Tab
{
    Q_OBJECT
public:
    Project(Zipper<Project *> *, const QString &path);
    virtual ~Project();
    View *activeView() const { return _activeView; }
    int activeViewIndex() const { return indexOf(_activeView); }
    bool setActiveViewIndex(int); // return whether active view changed
    const QList<View *> &views() const { return _views; }
    const QString &path() const { return _path; };
    void setPath(const QString &path);
    void unselect();
    void select();
    int indexOf(View *v) const { return _views.indexOf(v); }
    View *at(int i) const { return _views[i]; }
    int size() { return _views.size(); }
    void createEmptyViewAt(int);
    void removeAt(int);
    void removeView(View *view) { removeAt(indexOf(view)); }
    void cloneAt(int);
    void cloneView(View *view) { cloneAt(indexOf(view)); }
    void openFilesAt(int, const QStringList &);
    void resetViewHeaderSubtitles(int index, int total);
Q_SIGNALS:
    void pathChanged(const QString &);
    void viewInserted(int index, View *);
    void viewRemoved(View *);
    // this is emitted either the index or the view has changed, or both
    // XXX: the above line is kinda a lie, because it doesn't pre-emptively
    // emit signals e.g., when some view inserted
    // instead the signal is only emitted when one of its public functions
    // is called i.e. openFilesAt or createEmptyViewAt
    void activeViewChanged(int index, View *);
    void translatorChanged();
private:
    Zipper<Project *> *_projects;
    View *_activeView;
    QList<View *> _views;
    QString _path;

    void insertNewView(int index, Buffer *);
    void resetViewHeaderSubtitles();
    void resetViewHeaderTitles();
    Q_SLOT void onViewTriggered();
};

#endif /* PROJECT_H_ */
