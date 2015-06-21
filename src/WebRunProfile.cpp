/*
 * WebRunProfile.cpp
 *
 *  Created on: May 27, 2015
 *      Author: lingnan
 */

#include <bb/cascades/WebView>
#include <bb/cascades/WebSettings>
#include <bb/cascades/ScrollView>
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

WebRunProfile::WebRunProfile(View *view, WebRunProfile::Mode mode):
    RunProfile(view),
    _mode(mode),
    _webView(WebView::create()),
    _backButton(ActionItem::create()
        .addShortcut(Shortcut::create().key("x"))
        .onTriggered(view->content(), SLOT(pop()))),
    _backAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_backward.png"))
        .addShortcut(Shortcut::create().key("p"))
        .onTriggered(_webView, SLOT(goBack()))),
    _forwardAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_forward.png"))
        .addShortcut(Shortcut::create().key("n"))
        .onTriggered(_webView, SLOT(goForward()))),
    _rerunAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_reload.png"))
        .addShortcut(Shortcut::create().key("r"))
        .onTriggered(this, SLOT(rerun())))
{
//    _webView->saveImageAction()->setEnabled(false);
//    _webView->shareImageAction()->setEnabled(false);
    _webView->openLinkInNewTabAction()->setEnabled(false);
    _webView->settings()->setDefaultFontSize(25);
    ScrollView *scrollView = ScrollView::create()
        .scrollMode(ScrollMode::Both)
        .pinchToZoomEnabled(true)
        .content(_webView);
    conn(_webView, SIGNAL(minContentScaleChanged(float)),
        scrollView->scrollViewProperties(), SLOT(setMinContentScale(float)));
    conn(_webView, SIGNAL(maxContentScaleChanged(float)),
        scrollView->scrollViewProperties(), SLOT(setMaxContentScale(float)));
    conn(_webView, SIGNAL(navigationHistoryChanged()),
        this, SLOT(onNavigationHistoryChanged()));
    _outputPage = Page::create()
        .titleBar(TitleBar::create())
        .content(scrollView)
        .actionBarVisibility(ChromeVisibility::Overlay)
        .addAction(_backAction, ActionBarPlacement::OnBar)
        .addAction(_rerunAction, ActionBarPlacement::Signature)
        .addAction(_forwardAction, ActionBarPlacement::OnBar)
        .paneProperties(NavigationPaneProperties::create()
            .backButton(_backButton));
    _outputPage->setActionBarAutoHideBehavior(ActionBarAutoHideBehavior::HideOnScroll);
    _outputPage->setParent(this);

    onNavigationHistoryChanged();
    onTranslatorChanged();
    conn(view, SIGNAL(translatorChanged()), this, SLOT(onTranslatorChanged()));
}

void WebRunProfile::run()
{
    _outputPage->setParent(NULL);
    view()->content()->push(_outputPage);
    rerun();
}

void WebRunProfile::rerun()
{
    _webView->setHtml("");
    switch (_mode) {
        case WebRunProfile::Html:
            _webView->setHtml(view()->buffer()->state().plainText());
            break;
        case WebRunProfile::Javascript:
            _webView->evaluateJavaScript(view()->buffer()->state().plainText());
            break;
    }
}

void WebRunProfile::setMode(WebRunProfile::Mode mode)
{
    if (mode != _mode) {
        _mode = mode;
    }
}

void WebRunProfile::onNavigationHistoryChanged()
{
    _backAction->setEnabled(_webView->canGoBack());
    _forwardAction->setEnabled(_webView->canGoForward());
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
    _outputPage->titleBar()->setTitle(tr("View Page"));
    _backButton->setTitle(tr("Back to Editor"));
    _backAction->setTitle(tr("Back"));
    _rerunAction->setTitle(tr("Rerun"));
    _forwardAction->setTitle(tr("Forward"));
}
