/*
 * GeneralSettingsPage.cpp
 *
 *  Created on: May 25, 2015
 *      Author: lingnan
 */

#include <bb/cascades/TitleBar>
#include <bb/cascades/Picker>
#include <bb/cascades/Label>
#include <GeneralSettingsPage.h>
#include <GeneralSettings.h>
#include <HighlightRangePicker.h>
#include <Segment.h>
#include <Utility.h>
#include <Defaults.h>
#include <SignalBlocker.h>

using namespace bb::cascades;

GeneralSettingsPage::GeneralSettingsPage(GeneralSettings *generalSettings, QObject *parent):
    RepushablePage(parent),
    _title(TitleBar::create()),
    _picker(new HighlightRangePicker),
    _help(Label::create().multiline(true)
        .textStyle(Defaults::helpText()))
{
    setTitleBar(_title);

    onHighlightRangeChanged(generalSettings->highlightRange());
    conn(_picker, SIGNAL(selectedHighlightRangeChanged(int)),
        generalSettings, SLOT(setHighlightRange(int)));
    conn(generalSettings, SIGNAL(highlightRangeChanged(int)),
        this, SLOT(onHighlightRangeChanged(int)));

    setContent(Segment::create().section()
        .add(Segment::create().subsection()
            .add(_picker)));

    onTranslatorChanged();
}

void GeneralSettingsPage::onHighlightRangeChanged(int range)
{
    SignalBlocker blocker(_picker);
    _picker->setHighlightRange(range);
}

void GeneralSettingsPage::onTranslatorChanged()
{
    _title->setTitle(tr("General"));
    _picker->onTranslatorChanged();
    _help->setText(tr("Highlight range controls the number of lines to be highlighted on each side of the cursor. "
            "Adjusting this value down will improve performance and vice versa."));
}
