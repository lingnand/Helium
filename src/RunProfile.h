/*
 * RunProfile.h
 *
 *  Created on: May 9, 2015
 *      Author: lingnan
 */

#ifndef RUNPROFILE_H_
#define RUNPROFILE_H_

class View;

class RunProfile : public QObject
{
    Q_OBJECT
public:
    RunProfile(View *view);
    View *view() const;
    Q_SLOT virtual void run() = 0;
    virtual bool runnable() const = 0;
    virtual void exit() = 0;
Q_SIGNALS:
    void runnableChanged(bool);
};

#endif /* RUNPROFILE_H_ */
