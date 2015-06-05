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
    enum Type { None, Cmd, Web };
    static RunProfileManager *create(Type);
    static Type type(RunProfileManager *);
    virtual ~RunProfileManager() {};
    Filetype *parent() const;
    virtual RunProfile *createRunProfile(View *) = 0;
};
Q_DECLARE_METATYPE(RunProfileManager::Type)

#endif /* RUNPROFILEMANAGER_H_ */
