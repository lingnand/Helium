/*
 * WebRunProfile.cpp
 *
 *  Created on: May 27, 2015
 *      Author: lingnan
 */

#include <QTemporaryFile>
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
#include <hoedown/html.h>

using namespace bb::cascades;

WebRunProfile::WebRunProfile(View *view, WebRunProfile::Mode mode):
    RunProfile(view),
    _mode(mode),
    _webView(WebView::create()),
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
        .onTriggered(this, SLOT(rerun()))),
    _backButton(ActionItem::create()
        .addShortcut(Shortcut::create().key("x"))
        .onTriggered(view->content(), SLOT(pop()))),
    _hoedown_renderer(NULL), _hoedown_document(NULL), _hoedown_buffer(NULL)
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

WebRunProfile::~WebRunProfile()
{
    if (_hoedown_buffer)
        hoedown_buffer_free(_hoedown_buffer);
    if (_hoedown_document)
        hoedown_document_free(_hoedown_document);
    if (_hoedown_renderer)
        hoedown_html_renderer_free(_hoedown_renderer);
}

void WebRunProfile::run()
{
    _outputPage->setParent(NULL);
    view()->content()->push(_outputPage);
    rerun();
}

void WebRunProfile::rerun()
{
    if (!_temp.open()) {
        Utility::toast(tr("Failed to store output. Please try again."));
        return;
    }
    _temp.resize(0);
    switch (_mode) {
        case WebRunProfile::Html: {
            QTextStream out(&_temp);
            view()->buffer()->state().writePlainText(out);
            out.flush();
            break;
        }
        case WebRunProfile::Javascript:
            _webView->evaluateJavaScript(view()->buffer()->state().plainText());
            break;
        case WebRunProfile::Markdown: {
            if (!_hoedown_renderer) {
                _hoedown_renderer = hoedown_html_renderer_new(
                        (hoedown_html_flags) 0, 0);
            }
            if (!_hoedown_document) {
                unsigned int exts = HOEDOWN_EXT_TABLES
                        | HOEDOWN_EXT_FENCED_CODE
                        | HOEDOWN_EXT_FOOTNOTES
                        | HOEDOWN_EXT_AUTOLINK
                        | HOEDOWN_EXT_STRIKETHROUGH;
                _hoedown_document = hoedown_document_new(_hoedown_renderer,
                        (hoedown_extensions) exts, 16);
            }
            if (!_hoedown_buffer) {
                _hoedown_buffer = hoedown_buffer_new(16);
            } else {
                hoedown_buffer_reset(_hoedown_buffer);
            }
            QByteArray text = view()->buffer()->state().plainText().toUtf8();
            hoedown_document_render(_hoedown_document, _hoedown_buffer,
                    (const uint8_t *) text.constData(), text.size());
            QTextStream out(&_temp);
            out << "<html>";
            out << "<head>";
            out << "<meta charset='UTF-8'>";
            out << "<script type='text/x-mathjax-config'>";
            out << "MathJax.Hub.Config({";
            out << "    messageStyle: 'none',";
            out << "    extensions: ['tex2jax.js'],";
            out << "    jax: ['input/TeX', 'output/HTML-CSS'],";
            out << "    tex2jax: {";
            out << "      inlineMath: [['$','$']],";
            out << "      displayMath: [['$$','$$']],";
            out << "      processEscapes: true,";
            out << "      preview: 'none'";
            out << "    },";
            out << "    'HTML-CSS': { availableFonts: ['TeX'] }";
            out << "});";
            out << "</script>";
            out << QString("<script type='text/javascript' src='%1'></script>").arg(
                    QDir::currentPath()+"/app/native/assets/mathjax/MathJax.js");
            out << "</head>";
            out << "<body>";
            out << QByteArray((const char *) _hoedown_buffer->data, _hoedown_buffer->size);
            out << "</body>";
            out << "</head>";
            out << "</html>";
            out << flush;
            break;
        }
    }
    _temp.close();
    _webView->loadFile(_temp.fileName());
    _webView->reload();
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
    _backAction->setTitle(tr("Go Back"));
    _rerunAction->setTitle(tr("Rerun"));
    _forwardAction->setTitle(tr("Go Forward"));
    _backButton->setTitle(tr("Back"));
}
