/*
 * ViewMode.h
 *
 *  Created on: May 3, 2015
 *      Author: lingnan
 */

#ifndef VIEWMODE_H_
#define VIEWMODE_H_

class View;

class ViewMode: public QObject
{
    Q_OBJECT
public:
    ViewMode(View *);
    View *view() const;
    virtual void autoFocus(bool goToModeControl=false) = 0;
    virtual void onEnter() = 0;
    virtual void onExit() = 0;
};

#endif /* VIEWMODE_H_ */
