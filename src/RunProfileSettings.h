/*
 * RunProfileSettings.h
 *
 *  Created on: May 24, 2015
 *      Author: lingnan
 */

#ifndef RUNPROFILESETTINGS_H_
#define RUNPROFILESETTINGS_H_

#include <bb/cascades/Container>

class RunProfileManager;

class RunProfileSettings : public bb::cascades::Container
{
    Q_OBJECT
public:
    static RunProfileSettings *create(RunProfileManager *runProfileManager);
    virtual ~RunProfileSettings() {}
    Q_SLOT virtual void onTranslatorChanged() = 0;
};

#endif /* RUNPROFILESETTINGS_H_ */
