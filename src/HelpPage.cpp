/*
 * HelpPage.cpp
 *
 *  Created on: Jun 7, 2015
 *      Author: lingnan
 */

#include <bb/cascades/TitleBar>
#include <bb/cascades/ScrollView>
#include <bb/cascades/WebView>
#include <bb/cascades/WebSettings>
#include <bb/cascades/Option>
#include <bb/cascades/SystemDefaults>
#include <HelpPage.h>
#include <Utility.h>

using namespace bb::cascades;

HelpPage::HelpPage(QObject *parent):
    RepushablePage(parent),
    _quickStartOption(Option::create()
        .value(QUrl("local:///assets/docs/quick_start.html"))),
    _keyboardShortcutsOption(Option::create()
        .value(QUrl("local:///assets/docs/keyboard_shortcuts.html"))),
    _runProfileOption(Option::create()
        .value(QUrl("local:///assets/docs/run_profiles.html"))),
    _webView(WebView::create())
{
    _webView->saveImageAction()->setEnabled(false);
    _webView->shareImageAction()->setEnabled(false);
    _webView->openLinkInNewTabAction()->setEnabled(false);
    _webView->settings()->setDefaultFontSize(25);

    setTitleBar(TitleBar::create(TitleBarKind::Segmented)
        .addOption(_quickStartOption)
        .addOption(_keyboardShortcutsOption)
        .addOption(_runProfileOption));
    onTitleSelectionChanged(titleBar()->selectedValue());
    conn(titleBar(), SIGNAL(selectedValueChanged(const QVariant&)),
        this, SLOT(onTitleSelectionChanged(const QVariant&)));

    setContent(ScrollView::create()
        .scrollMode(ScrollMode::Vertical)
        .content(_webView));

    onTranslatorChanged();
}

void HelpPage::onTitleSelectionChanged(const QVariant &v)
{
    _webView->setUrl(v.toUrl());
}

void HelpPage::onTranslatorChanged()
{
    _quickStartOption->setText(tr("Quick Start"));
    _keyboardShortcutsOption->setText(tr("Keyboard Shortcuts"));
    _runProfileOption->setText(tr("Run Profiles"));
}

