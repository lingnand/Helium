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
#include <NumberPicker.h>
#include <Segment.h>
#include <Utility.h>
#include <Defaults.h>

using namespace bb::cascades;

GeneralSettingsPage::GeneralSettingsPage(GeneralSettings *generalSettings, QObject *parent):
    RepushablePage(parent),
    _picker(new NumberPicker(0, 40)),
    _help(Label::create().multiline(true)
        .textStyle(Defaults::helpText()))
{
    setTitleBar(TitleBar::create());

    _picker->setSelectedNumber(generalSettings->highlightRange());
    conn(_picker, SIGNAL(selectedNumberChanged(int)),
        generalSettings, SLOT(setHighlightRange(int)));
    conn(generalSettings, SIGNAL(highlightRangeChanged(int)),
        _picker, SLOT(setSelectedNumber(int)));

    setContent(Segment::create().section()
        .add(Segment::create().subsection().add(_picker))
        .add(Segment::create().subsection().add(_help)));

    onTranslatorChanged();
}

void GeneralSettingsPage::onTranslatorChanged()
{
    titleBar()->setTitle(tr("General"));
    _picker->setTitle(tr("Highlight Range"));
    _help->setText(tr("Highlight range controls the number of lines to be highlighted on each side of the cursor. "
            "Adjusting this value down will improve performance and vice versa."));
}
