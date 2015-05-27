/*
 * RunProfileSettingsUI.h
 *
 *  Created on: May 24, 2015
 *      Author: lingnan
 */

#ifndef RUNPROFILESETTINGSUI_H_
#define RUNPROFILESETTINGSUI_H_

#include <bb/cascades/Container>

class RunProfileManager;

class RunProfileSettingsUI : public bb::cascades::Container
{
    Q_OBJECT
public:
    static RunProfileSettingsUI *create(RunProfileManager *runProfileManager);
    virtual ~RunProfileSettingsUI() {}
    Q_SLOT virtual void onTranslatorChanged() = 0;
};

#endif /* RUNPROFILESETTINGSUI_H_ */
