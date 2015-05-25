/*
 * GeneralSettings.cpp
 *
 *  Created on: May 25, 2015
 *      Author: lingnan
 */

#include <bb/cascades/TitleBar>
#include <bb/cascades/Picker>
#include <bb/cascades/Label>
#include <Segment.h>
#include <GeneralSettings.h>
#include <HighlightRangePicker.h>
#include <Utility.h>
#include <Defaults.h>

using namespace bb::cascades;

GeneralSettings::GeneralSettings():
    _title(TitleBar::create()),
    _picker(new HighlightRangePicker),
    _help(Label::create().multiline(true)
        .textStyle(Defaults::helpText()))
{
    setTitleBar(_title);
    conn(_picker, SIGNAL(highlightRangeChanged(int)),
        this, SIGNAL(highlightRangeChanged(int)));
    setContent(Segment::create().section()
        .add(Segment::create().subsection()
            .add(_picker)));
    onTranslatorChanged();
}

void GeneralSettings::onTranslatorChanged()
{
    _title->setTitle(tr("General"));
    _picker->onTranslatorChanged();
    _help->setText(tr("Highlight range controls the number of lines to be highlighted on each side of the cursor. "
            "Adjusting this value down will improve performance and vice versa."));
}
