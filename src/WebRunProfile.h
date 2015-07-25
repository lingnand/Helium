/*
 * WebRunProfile.h
 *
 *  Created on: May 27, 2015
 *      Author: lingnan
 */

#ifndef WEBRUNPROFILE_H_
#define WEBRUNPROFILE_H_

#include <QTemporaryFile>
#include <RunProfile.h>

namespace bb {
    namespace cascades {
        class WebView;
        class Page;
        class ActionItem;
    }
}

class hoedown_renderer;
class hoedown_document;
class hoedown_buffer;

class WebRunProfile : public RunProfile
{
    Q_OBJECT
public:
    enum Mode { Html, Javascript, Markdown };
    WebRunProfile(View *, Mode);
    virtual ~WebRunProfile();
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
    bb::cascades::ActionItem *_backButton;
    QTemporaryFile *_temp;
    // hoedown stuff
    hoedown_renderer *_hoedown_renderer;
    hoedown_document *_hoedown_document;
    hoedown_buffer *_hoedown_buffer;
    Q_SLOT void rerun();
    Q_SLOT void onNavigationHistoryChanged();
    Q_SLOT void onBufferFilepathChanged(const QString &);
};

#endif /* WEBRUNPROFILE_H_ */
