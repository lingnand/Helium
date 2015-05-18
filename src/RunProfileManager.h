/*
 * RunProfileManager.h
 *
 *  Created on: May 15, 2015
 *      Author: lingnan
 */

#ifndef RUNPROFILEMANAGER_H_
#define RUNPROFILEMANAGER_H_

class Filetype;
class RunProfile;
class View;

class RunProfileManager : public QObject
{
    Q_OBJECT
public:
    Filetype *parent() const;
    virtual RunProfile *createRunProfile(View *) = 0;
};

#endif /* RUNPROFILEMANAGER_H_ */
