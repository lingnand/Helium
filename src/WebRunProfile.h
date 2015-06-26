/*
 * WebRunProfile.h
 *
 *  Created on: May 27, 2015
 *      Author: lingnan
 */

#ifndef WEBRUNPROFILE_H_
#define WEBRUNPROFILE_H_

#include <RunProfile.h>

namespace bb {
    namespace cascades {
        class WebView;
        class Page;
        class ActionItem;
    }
}

class WebRunProfile : public RunProfile
{
    Q_OBJECT
public:
    enum Mode { Html, Javascript };
    WebRunProfile(View *, Mode);
    virtual ~WebRunProfile() {}
    void run();
    bool runnable() const { return true; } // always runnable
    Q_SLOT void setMode(WebRunProfile::Mode);
    void exit();
    Q_SLOT void onTranslatorChanged();
private:
    Mode _mode;
    bb::cascades::Page *_outputPage;
    bb::cascades::WebView *_webView;
    bb::cascades::ActionItem *_backAction;
    bb::cascades::ActionItem *_forwardAction;
    bb::cascades::ActionItem *_rerunAction;
    Q_SLOT void rerun();
    Q_SLOT void onNavigationHistoryChanged();
};

#endif /* WEBRUNPROFILE_H_ */
