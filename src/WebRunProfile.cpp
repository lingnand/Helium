/*
 * WebRunProfile.cpp
 *
 *  Created on: May 27, 2015
 *      Author: lingnan
 */

#include <QRectF>
#include <bb/cascades/WebView>
#include <bb/cascades/ScrollView>
#include <bb/cascades/LayoutUpdateHandler>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/NavigationPaneProperties>
#include <bb/cascades/ActionItem>
#include <bb/cascades/Shortcut>
#include <bb/cascades/TitleBar>
#include <WebRunProfile.h>
#include <View.h>
#include <Buffer.h>
#include <Utility.h>

using namespace bb::cascades;

// TODO: make the page title display the title of the webpage
//       (alternatively, don't display any title at all), just show the full view
// TODO: add back and forward, reload actions?
// TODO: make the action bar hideable (like in other apps)
//       this can probably be used for CmdRunProfile as well
WebRunProfile::WebRunProfile(View *view):
    RunProfile(view),
    _webView(WebView::create()),
    _backButton(ActionItem::create()
        .addShortcut(Shortcut::create().key("x"))
        .onTriggered(view->content(), SLOT(pop())))
{
//    _webView->saveImageAction()->setEnabled(false);
//    _webView->shareImageAction()->setEnabled(false);
    _webView->openLinkInNewTabAction()->setEnabled(false);
    ScrollView *scrollView = ScrollView::create()
        .scrollMode(ScrollMode::Both)
        .pinchToZoomEnabled(true)
        .content(_webView);
    LayoutUpdateHandler::create(scrollView)
        .onLayoutFrameChanged(this, SLOT(onScrollViewLayoutFrameChanged(const QRectF&)));
    conn(_webView, SIGNAL(minContentScaleChanged(float)),
        scrollView->scrollViewProperties(), SLOT(setMinContentScale(float)));
    conn(_webView, SIGNAL(maxContentScaleChanged(float)),
        scrollView->scrollViewProperties(), SLOT(setMaxContentScale(float)));
    _outputPage = Page::create()
        .titleBar(TitleBar::create())
        .content(scrollView)
        .paneProperties(NavigationPaneProperties::create()
            .backButton(_backButton));
    _outputPage->setParent(this);

    onTranslatorChanged();
    conn(view, SIGNAL(translatorChanged()), this, SLOT(onTranslatorChanged()));
}

void WebRunProfile::onScrollViewLayoutFrameChanged(const QRectF &rect)
{
    _webView->setPreferredHeight(rect.height());
}

void WebRunProfile::run()
{
    _outputPage->setParent(NULL);
    view()->content()->push(_outputPage);
    _webView->setHtml(view()->buffer()->state().plainText());
}

void WebRunProfile::exit()
{
    if (view()->content()->top() == _outputPage) {
        view()->content()->pop();
    }
    _outputPage->setParent(this);
}

void WebRunProfile::onTranslatorChanged()
{
    _outputPage->titleBar()->setTitle("View Page");
}
