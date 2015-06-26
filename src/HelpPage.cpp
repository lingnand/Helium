/*
 * HelpPage.cpp
 *
 *  Created on: Jun 7, 2015
 *      Author: lingnan
 */

#include <bb/cascades/TitleBar>
#include <bb/cascades/ScrollView>
#include <bb/cascades/Header>
#include <bb/cascades/Label>
#include <bb/cascades/SystemDefaults>
#include <HelpPage.h>
#include <Utility.h>
#include <Segment.h>

using namespace bb::cascades;

HelpPage::HelpPage(QObject *parent):
    RepushablePage(parent),
    _tabsAndBuffers(Header::create().mode(HeaderMode::Interactive)
        .onClicked(this, SLOT(onTabsAndBuffersHeaderClicked()))),
    _tabsAndBuffersDesc(Label::create().multiline(true).format(TextFormat::Html)
        .textStyle(SystemDefaults::TextStyles::bodyText())),
    _fileManagement(Header::create().mode(HeaderMode::Interactive)
        .onClicked(this, SLOT(onFileManagementHeaderClicked()))),
    _fileManagementDesc(Label::create().multiline(true).format(TextFormat::Html)
        .textStyle(SystemDefaults::TextStyles::bodyText())),
    _highlight(Header::create().mode(HeaderMode::Interactive)
        .onClicked(this, SLOT(onHighlightHeaderClicked()))),
    _highlightDesc(Label::create().multiline(true).format(TextFormat::Html)
        .textStyle(SystemDefaults::TextStyles::bodyText())),
    _filetype(Header::create().mode(HeaderMode::Interactive)
        .onClicked(this, SLOT(onFiletypeHeaderClicked()))),
    _filetypeDesc(Label::create().multiline(true).format(TextFormat::Html)
        .textStyle(SystemDefaults::TextStyles::bodyText())),
    _findAndReplace(Header::create().mode(HeaderMode::Interactive)
        .onClicked(this, SLOT(onFindAndReplaceHeaderClicked()))),
    _findAndReplaceDesc(Label::create().multiline(true).format(TextFormat::Html)
        .textStyle(SystemDefaults::TextStyles::bodyText())),
    _contentPage(NULL)
{
    setTitleBar(TitleBar::create());

    setContent(ScrollView::create()
        .scrollMode(ScrollMode::Vertical)
        .content(Segment::create().section()
            .add(_tabsAndBuffers)
            .add(Segment::create().subsection().add(_tabsAndBuffersDesc))
            .add(_fileManagement)
            .add(Segment::create().subsection().add(_fileManagementDesc))
            .add(_highlight)
            .add(Segment::create().subsection().add(_highlightDesc))
            .add(_filetype)
            .add(Segment::create().subsection().add(_filetypeDesc))
            .add(_findAndReplace)
            .add(Segment::create().subsection().add(_findAndReplaceDesc))));
    setActionBarVisibility(ChromeVisibility::Overlay);
    setActionBarAutoHideBehavior(ActionBarAutoHideBehavior::HideOnScroll);

    onTranslatorChanged();
}

HelpPage::ContentPage::ContentPage(QObject *parent):
    RepushablePage(parent),
    contentLabel(Label::create().multiline(true).format(TextFormat::Html)
        .textStyle(SystemDefaults::TextStyles::bodyText()))
{
    setTitleBar(TitleBar::create());
    setContent(ScrollView::create()
        .scrollMode(ScrollMode::Vertical)
        .content(Segment::create().section().subsection()
            .add(contentLabel)));
    setActionBarVisibility(ChromeVisibility::Overlay);
    setActionBarAutoHideBehavior(ActionBarAutoHideBehavior::HideOnScroll);
}

void HelpPage::loadPage(const QString &title, const QString &content)
{
    if (!_contentPage) {
        _contentPage = new HelpPage::ContentPage(this);
        conn(_contentPage, SIGNAL(toPush(bb::cascades::Page*)),
            this, SIGNAL(toPush(bb::cascades::Page*)));
        conn(_contentPage, SIGNAL(toPop()),
            this, SIGNAL(toPop()));
    }
    _contentPage->titleBar()->setTitle(title);
    _contentPage->contentLabel->setText(content);
    _contentPage->setParent(NULL);
    emit toPush(_contentPage);
}

void HelpPage::onTabsAndBuffersHeaderClicked()
{
    loadPage(_tabsAndBuffers->title(), tr("To be continued"));
}

void HelpPage::onFileManagementHeaderClicked()
{
    loadPage(_fileManagement->title(), tr("To be continued"));
}

void HelpPage::onHighlightHeaderClicked()
{
    loadPage(_highlight->title(), tr("To be continued"));
}

void HelpPage::onFiletypeHeaderClicked()
{
    loadPage(_filetype->title(), tr("To be continued"));
}

void HelpPage::onFindAndReplaceHeaderClicked()
{
    loadPage(_findAndReplace->title(), tr("To be continued"));
}

void HelpPage::onTranslatorChanged()
{
    titleBar()->setTitle(tr("Help"));
    _tabsAndBuffers->setTitle(tr("Tabs & Buffers"));
    _tabsAndBuffers->setSubtitle(tr("More"));
    _tabsAndBuffersDesc->setText("Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");
    _fileManagement->setTitle(tr("File Management"));
    _fileManagement->setSubtitle(tr("More"));
    _fileManagementDesc->setText("Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");
    _highlight->setTitle(tr("Highlight"));
    _highlight->setSubtitle(tr("More"));
    _highlightDesc->setText("Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");
    _filetype->setTitle(tr("Filetype"));
    _filetype->setSubtitle(tr("More"));
    _filetypeDesc->setText("Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");
    _findAndReplace->setTitle(tr("Find & Replace"));
    _findAndReplace->setSubtitle(tr("More"));
    _findAndReplaceDesc->setText("Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");
}

