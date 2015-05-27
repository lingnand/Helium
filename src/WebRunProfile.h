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
    WebRunProfile(View *);
    virtual ~WebRunProfile() {}
    void run();
    bool runnable() const {
        return true; // always runnable
    }
    void exit();
    Q_SLOT void onTranslatorChanged();
private:
    bb::cascades::Page *_outputPage;
    bb::cascades::WebView *_webView;
    bb::cascades::ActionItem *_backButton;
    Q_SLOT void onScrollViewLayoutFrameChanged(const QRectF&);
};

#endif /* WEBRUNPROFILE_H_ */
