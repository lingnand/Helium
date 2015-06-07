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

using namespace bb::cascades;

GeneralSettingsPage::GeneralSettingsPage(GeneralSettings *generalSettings, QObject *parent):
    RepushablePage(parent),
    _picker(new HighlightRangePicker),
    _help(Label::create().multiline(true)
        .textStyle(Defaults::helpText()))
{
    setTitleBar(TitleBar::create());

    _picker->setHighlightRange(generalSettings->highlightRange());
    conn(_picker, SIGNAL(selectedHighlightRangeChanged(int)),
        generalSettings, SLOT(setHighlightRange(int)));
    conn(generalSettings, SIGNAL(highlightRangeChanged(int)),
        _picker, SLOT(setHighlightRange(int)));

    setContent(Segment::create().section()
        .add(Segment::create().subsection()
            .add(_picker))
        .add(Segment::create().subsection()
            .add(_help)));

    onTranslatorChanged();
}

void GeneralSettingsPage::onTranslatorChanged()
{
    titleBar()->setTitle(tr("General"));
    _picker->onTranslatorChanged();
    _help->setText(tr("Highlight range controls the number of lines to be highlighted on each side of the cursor. "
            "Adjusting this value down will improve performance and vice versa."));
}
