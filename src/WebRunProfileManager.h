/*
 * WebRunProfileManager.h
 *
 *  Created on: Jun 4, 2015
 *      Author: lingnan
 */

#ifndef WEBRUNPROFILEMANAGER_H_
#define WEBRUNPROFILEMANAGER_H_

#include <RunProfileManager.h>
#include <WebRunProfile.h>

class WebRunProfileManager : public RunProfileManager
{
    Q_OBJECT
public:
    WebRunProfileManager(WebRunProfile::Mode=WebRunProfile::Html);
    virtual ~WebRunProfileManager() {}
    RunProfile *createRunProfile(View *);
    WebRunProfile::Mode mode() const { return _mode; }
    Q_SLOT void setMode(WebRunProfile::Mode);
Q_SIGNALS:
    void modeChanged(WebRunProfile::Mode);
private:
    WebRunProfile::Mode _mode;
};

#endif /* WEBRUNPROFILEMANAGER_H_ */
