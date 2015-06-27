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
#include <ShortcutHelp.h>
#include <Utility.h>
#include <Segment.h>

using namespace bb::cascades;

#define BULLET_SYMBOL QChar(0x2022)

HelpPage::HelpPage(QObject *parent):
    RepushablePage(parent),
    _tabsAndBuffers(Header::create().mode(HeaderMode::Interactive)
        .onClicked(this, SLOT(onTabsAndBuffersHeaderClicked()))),
    _tabsAndBuffersDesc(Label::create().multiline(true).format(TextFormat::Html)
        .textStyle(SystemDefaults::TextStyles::bodyText())),
    _fileManagement(Header::create().mode(HeaderMode::Interactive)),
    _fileManagementDesc(Label::create().multiline(true).format(TextFormat::Html)
        .textStyle(SystemDefaults::TextStyles::bodyText())),
    _keyboardShortcuts(Header::create().mode(HeaderMode::Interactive)),
    _keyboardShortcutsDesc(Label::create().multiline(true).format(TextFormat::Html)
        .textStyle(SystemDefaults::TextStyles::bodyText())),
    _highlight(Header::create().mode(HeaderMode::Interactive)),
    _highlightDesc(Label::create().multiline(true).format(TextFormat::Html)
        .textStyle(SystemDefaults::TextStyles::bodyText())),
    _findAndReplace(Header::create().mode(HeaderMode::Interactive)),
    _findAndReplaceDesc(Label::create().multiline(true).format(TextFormat::Html)
        .textStyle(SystemDefaults::TextStyles::bodyText())),
    _filetype(Header::create().mode(HeaderMode::Interactive)),
    _filetypeDesc(Label::create().multiline(true).format(TextFormat::Html)
        .textStyle(SystemDefaults::TextStyles::bodyText())),
    _runProfile(Header::create().mode(HeaderMode::Interactive)),
    _runProfileDesc(Label::create().multiline(true).format(TextFormat::Html)
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
            .add(_keyboardShortcuts)
            .add(Segment::create().subsection().add(_keyboardShortcutsDesc))
            .add(_highlight)
            .add(Segment::create().subsection().add(_highlightDesc))
            .add(_findAndReplace)
            .add(Segment::create().subsection().add(_findAndReplaceDesc))
            .add(_filetype)
            .add(Segment::create().subsection().add(_filetypeDesc))
            .add(_runProfile)
            .add(Segment::create().subsection().add(_runProfileDesc))));
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
    loadPage(_tabsAndBuffers->title(), tr(
        "<p>The distinction between <em>tabs</em> and <em>buffers</em> is necessary because Helium supports multiple tabs viewing the same buffer. This can be achieved in two ways:</p>"
        "<br/>"
        "  %1 use the <strong>Clone</strong> action inside any tab - this spawns a new tab that shares the same buffer with the previous tab<br/>"
        "  %1 opening a file in a new tab which is already edited in another tab causes the new tab to share that existing buffer<br/>"
        "<br/>"
        "<p>Changes in a buffer are synced across all tabs linked to it. Additionally, settings accessed and changed via any tab's <strong>Properties</strong> action apply to the same buffer. This means that you can for example:</p>"
        "<br/>"
        "  %1 edit a long document in one tab and refer back to its early parts in another tab<br/>"
        "  %1 edit a script in one tab and run &amp; view its output in another<br/>"
        "<br/>"
        "<p><em>Tips: a cloned tab shares the same working directory as the old one; so to open some other file in the same directory, you can clone the current tab and the Open action will navigate to that directory by default</em></p>"
        ).arg(BULLET_SYMBOL));
}

void HelpPage::onTranslatorChanged()
{
    titleBar()->setTitle(tr("Help"));
    _tabsAndBuffers->setTitle(tr("Tabs & Buffers"));
    _tabsAndBuffers->setSubtitle(tr("More"));
    _tabsAndBuffersDesc->setText(tr("At any time Helium manages a list of <em>tabs</em> which are views into <em>buffers</em>. A tab is a frontend that interacts with a buffer, whereas a buffer represents the actual data being edited i.e. what you see in the text area; and also data being saved to/loaded from files."));
    _fileManagement->setTitle(tr("File Management"));
    _fileManagementDesc->setText(tr("<p style='margin-bottom:2em'>Helium supports opening multiple files at once. Simply use the <strong>Open</strong> action in any tab, choose the files you desire to edit and select <strong>Done</strong>. If there are more than one file chosen, the additional files will be put into new tabs <em>just before</em> the current one.</p>"
        "Saving is done asynchronously i.e. every time you trigger the <strong>Save</strong> action, Helium copies the content in the current buffer, and starts a background routine that saves it to disk. Progress is displayed at the bottom of the tab in an non-interruptive way - this means that you can continue editing the document without having to wait for it to finish."));
    _keyboardShortcuts->setTitle(tr("Keyboard Shortcuts"));
    _keyboardShortcutsDesc->setText(tr("<p>Helium supports a wide range of keyboard shortcuts from the normal ones to the ones triggered directly while editing text via modifiers (press the shortcut key while holding %3). At any time you can view the full list of shortcuts by pressing:</p>"
        "  %1 %2 when nothing is in focus<br/>"
        "  %1 %3 %2 when inside a text field or text area"
        ).arg(BULLET_SYMBOL, BACKSPACE_SYMBOL, RETURN_SYMBOL));
    _highlight->setTitle(tr("Highlight"));
    _highlightDesc->setText(tr("<p>Helium uses efficient algorithms to highlight documents in real-time. However, due to limitations on the amount of highlighted text that can be put into a text area, Helium uses <em>partial highlight</em> instead to keep highlight to the necessary minimum (the range of text around the cursor). If you still encounter performance problems, you can</p>"
        "  %1 turn off <strong>Enable Highlight</strong> for the given filetype, OR<br/>"
        "  %1 decrease the range of partial highlight in <strong>General Settings</strong>"
        ).arg(BULLET_SYMBOL));
    _findAndReplace->setTitle(tr("Find & Replace"));
    _findAndReplaceDesc->setText(tr("<p>Use the <strong>Find</strong> action to enter <em>Find Mode</em>. Helium supports regex find and replace with three different styles (accessed as tab options or via keyboard shortcuts)</p>"
        "  %1 <strong>Full Regex</strong>: see <a href='http://perldoc.perl.org/perlre.html#Regular-Expressions'>http://perldoc.perl.org/perlre.html#Regular-Expressions</a> for Perl-style regular expressions<br/>"
        "  %1 <strong>Ignore Case</strong>: similar to <em>Full Regex</em> but ignore the case for letters<br/>"
        "  %1 <strong>Exact Match</strong>: all characters are taken literally<br/>"
        "Replacments support capturing groups as well, e.g. replacing <span style='color:orange'>([^a-z]*)$</span> with <span style='color:orange'>\\n\\1</span> will put non-algebraic characters at the end of each line into a new line"
        ).arg(BULLET_SYMBOL));
    _filetype->setTitle(tr("Filetype"));
    _filetypeDesc->setText(tr("Each filetype defines its own set of highlight rules, as well as having its own settings for highlight and <em>run profiles</em>. These settings can be accessed via <strong>Properties</strong> inside any tab (which shows settings of the filetype currently associated with the given buffer); they can also be accessed inside <strong>Settings-Filetypes</strong>."));
    _runProfile->setTitle(tr("Run Profile"));
    _runProfileDesc->setText(tr("<p><em>Run profiles</em> can be configured for each filetype inside <strong>Properties</strong> or <strong>Settings-Filetypes</strong>. When editing a file of a filetype with an available run profile, you can use the <strong>Run</strong> action to run the current content. By default,  the following filetypes already have their run profiles defined:</p>"
        "  %1 <em>python, shell, html, javascript</em>"
        ).arg(BULLET_SYMBOL));
}

