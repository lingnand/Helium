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
#include <bb/cascades/Option>
#include <bb/cascades/ActionItem>
#include <bb/cascades/Sheet>
#include <bb/cascades/SegmentedControl>
#include <bb/cascades/Shortcut>
#include <HelpPage.h>
#include <ShortcutHelp.h>
#include <Utility.h>
#include <Segment.h>
#include <Helium.h>

using namespace bb::cascades;

#define BULLET_SYMBOL QChar(0x2022)

HelpPage::HelpPage():
    _referenceOption(Option::create()),
    _changeListOption(Option::create()),
    _tabsAndBuffers(Header::create().mode(HeaderMode::Interactive)
        .onClicked(this, SLOT(onTabsAndBuffersHeaderClicked()))),
    _tabsAndBuffersDesc(Label::create().multiline(true).format(TextFormat::Html)
        .textStyle(SystemDefaults::TextStyles::bodyText())),
    _projectsManagement(Header::create().mode(HeaderMode::Interactive)),
    _projectsManagementDesc(Label::create().multiline(true).format(TextFormat::Html)
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
    _referenceSegment(Segment::create().section()
        .add(_tabsAndBuffers)
        .add(Segment::create().subsection().add(_tabsAndBuffersDesc))
        .add(_projectsManagement)
        .add(Segment::create().subsection().add(_projectsManagementDesc))
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
        .add(Segment::create().subsection().add(_runProfileDesc))),
    _changeList(Label::create().multiline(true).format(TextFormat::Html)
        .textStyle(SystemDefaults::TextStyles::bodyText())),
    _changeListSegment(Segment::create().section().subsection()
        .add(_changeList)),
    _view(ScrollView::create(_referenceSegment)
        .scrollMode(ScrollMode::Vertical)
        .scrollRole(ScrollRole::Main)),
    _rateAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_browser.png"))
        .onTriggered(Helium::instance(), SLOT(goToAppWorld()))),
    _base(Page::create()
        .titleBar(TitleBar::create()
            .dismissAction(ActionItem::create()
                .onTriggered(this, SLOT(closeSheet()))))
        .addAction(_rateAction, ActionBarPlacement::OnBar)
        .addShortcut(Shortcut::create().key("x")
            .onTriggered(this, SLOT(closeSheet())))
        .actionBarVisibility(ChromeVisibility::Overlay)),
    _contentPage(NULL)
{
    SegmentedControl *segment = SegmentedControl::create()
        .add(_referenceOption)
        .add(_changeListOption);
    conn(segment, SIGNAL(selectedOptionChanged(bb::cascades::Option*)),
        this, SLOT(onSelectedOptionChanged(bb::cascades::Option*)));

    _base->setContent(Segment::create().section()
        .add(segment).add(_view));
    _base->setActionBarAutoHideBehavior(ActionBarAutoHideBehavior::HideOnScroll);
    push(_base);

    onTranslatorChanged();
}

HelpPage::ContentPage::ContentPage():
    contentLabel(Label::create().multiline(true).format(TextFormat::Html)
        .textStyle(SystemDefaults::TextStyles::bodyText()))
{
    setTitleBar(TitleBar::create());
    setContent(ScrollView::create(Segment::create().section().subsection()
            .add(contentLabel))
        .scrollMode(ScrollMode::Vertical));
    setActionBarVisibility(ChromeVisibility::Overlay);
    setActionBarAutoHideBehavior(ActionBarAutoHideBehavior::HideOnScroll);
}

void HelpPage::loadPage(const QString &title, const QString &content)
{
    if (!_contentPage) {
        _contentPage = new HelpPage::ContentPage;
    }
    _contentPage->titleBar()->setTitle(title);
    _contentPage->contentLabel->setText(content);
    push(_contentPage);
}

void HelpPage::onSelectedOptionChanged(bb::cascades::Option *option)
{
    if (option == _referenceOption)
        _view->setContent(_referenceSegment);
    else if (option == _changeListOption)
        _view->setContent(_changeListSegment);
}

void HelpPage::setMode(HelpPage::Mode mode)
{
    switch (mode) {
        case HelpPage::Reference:
            _base->titleBar()->setSelectedOption(_referenceOption); break;
        case HelpPage::ChangeList:
            _base->titleBar()->setSelectedOption(_changeListOption); break;
    }
}

void HelpPage::closeSheet()
{
    ((Sheet *) parent())->close();
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
    _base->titleBar()->setTitle(tr("Help"));
    _base->titleBar()->dismissAction()->setTitle(tr("Close"));
    _referenceOption->setText(tr("Reference"));
    _changeListOption->setText(tr("Change List"));
    _tabsAndBuffers->setTitle(tr("Tabs & Buffers"));
    _tabsAndBuffers->setSubtitle(tr("More"));
    _tabsAndBuffersDesc->setText(tr("At any time Helium manages a list of <em>tabs</em> which are views into <em>buffers</em>. A tab is a frontend that interacts with a buffer, whereas a buffer represents the actual data being edited i.e. what you see in the text area; and also data being saved to/loaded from files."));
    _projectsManagement->setTitle(tr("Project Management"));
    _projectsManagementDesc->setText(tr("Each tab lives in a <em>project</em>, which is just a workspace with a working directory. Projects are listed at the top of the sidebar, with the active one having a ticked image besides it. Tapping another project switches to that project and displays the tabs opened in it, while tapping the active one brings up a file picker to change its working directory. Projects are useful for opening many files under the same directory and managing many tabs."));
    _fileManagement->setTitle(tr("File Management"));
    _fileManagementDesc->setText(tr("<p>Helium supports opening multiple files at once. Simply use the <strong>Open</strong> action in any tab, choose the files you desire to edit and select <strong>Done</strong>.</p>"
        "Saving is done asynchronously i.e. every time you trigger the <strong>Save</strong> action, Helium copies the content in the current buffer, and starts a background routine that saves it to disk. Progress is displayed at the bottom of the tab in an non-interruptive way - this means that you can continue editing the document without having to wait for it to finish."));
    _keyboardShortcuts->setTitle(tr("Keyboard Shortcuts"));
    _keyboardShortcutsDesc->setText(tr("Helium supports a wide range of keyboard shortcuts from the normal ones to the ones triggered directly while editing text via modifiers (press the shortcut key while holding %1). At any time (inside a text field/area or not) you can view the full list of shortcuts (where currently applicable) by pressing %1 %2"
        ).arg(RETURN_SYMBOL, BACKSPACE_SYMBOL));
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
        "Replacments support capturing groups as well, e.g. replacing <span style='color:orange'>([^a-z]*)$</span> with <span style='color:orange'>\\n\\1</span> will put non-alphabetic characters at the end of each line into a new line."
        ).arg(BULLET_SYMBOL));
    _filetype->setTitle(tr("Filetype"));
    _filetypeDesc->setText(tr("Each filetype defines its own set of highlight rules, as well as having its own settings for highlight and <em>run profiles</em>. These settings can be accessed via <strong>Properties</strong> inside any tab (which shows settings of the filetype currently associated with the given buffer); they can also be accessed inside <strong>Settings-Filetypes</strong>."));
    _runProfile->setTitle(tr("Run Profile"));
    _runProfileDesc->setText(tr("<p><em>Run profiles</em> can be configured for each filetype inside <strong>Properties</strong> or <strong>Settings-Filetypes</strong>. When editing a file of a filetype with an available run profile, you can use the <strong>Run</strong> action to run the current content. By default,  the following filetypes already have their run profiles defined:</p>"
        "  %1 <em>python, shell, html, javascript</em><br/>"
        "Note that the command run profile supports running <em>any</em> shell command. For example, you can compile gcc using <a href='https://github.com/mordak/playbook-dev-tools'>playbook-dev-tools</a> and define a command to compile and run your cpp files directly."
        ).arg(BULLET_SYMBOL));
    _changeList->setText(tr(
        "<strong><u>Version 1.0.4.x</u></strong><br/><br/>"
        "  %1 NEW: Full-screen mode and ability to hide title bar<br/>"
        "  %1 NEW: added <strong>Rename</strong> and <strong>Reload</strong> actions<br/>"
        "  %1 NEW: added auto-detection of external changes on files being edited<br/>"
        "  %1 NEW: instruction on how to compile and run c/c++ files within Helium (<strong>Reference-Run Profile</strong>)<br/>"
        "  %1 IMPROVE: make use of font settings for command output<br/><br/>"
        "<strong><u>Version 1.0.3.x</u></strong><br/><br/>"
        "  %1 NEW: project management - manage tabs with <em>projects</em> and open files within the project directory easily<br/>"
        "  %1 NEW: persistence for opened projects<br/>"
        "  %1 <strong>CHANGE</strong>: %3 (when text field/area is not focused) is now mapped to <em>Close Project</em>; <strong>keyboard shortcut help is now triggered by</strong> %2 %3 <strong>regardless of whether a text field/area is focused or not</strong><br/>"
        "  %1 IMPROVE: opening files now doesn't ask for <em>Unsaved change</em> confirmation anymore: it creates new tabs after the current one<br/>"
        "  %1 IMPROVE: new tab is now inserted after the current one instead of always appending to the end<br/>"
        "  %1 IMPROVE: keyboard shortcut help is now displayed via prompt instead of toast: too much information for a toast<br/><br/>"
        "<strong><u>Version 1.0.2.x</u></strong><br/><br/>"
        "  %1 NEW: space-tab conversion for each filetype<br/>"
        "  %1 IMPROVE: saving progress is displayed more clearly<br/>"
        "  %1 IMPROVE: web run profile now allows previewing files with relative path to other files e.g., images, style files<br/>"
        "  %1 IMPROVE: added SIGTERM control to command run profile - now programs can handle termination gracefully<br/>"
        "  %1 IMPROVE: changed python run command to output without buffering<br/>"
        "  %1 FIX: syntax highlight problem with files containing special control characters<br/><br/>"
        "<strong><u>Version 1.0.1.x</u></strong><br/><br/>"
        "  %1 NEW: Markdown syntax highlight support and previewing support<br/>"
        "  %1 NEW: Markdown previewing support for Math symbols<br/>"
        "  %1 FIX: Alt+Enter problem on some devices<br/>"
        "  %1 FIX: some pages are not scrollable<br/>"
        ).arg(BULLET_SYMBOL, RETURN_SYMBOL, BACKSPACE_SYMBOL));
    _rateAction->setTitle(tr("Rate Helium"));
}
