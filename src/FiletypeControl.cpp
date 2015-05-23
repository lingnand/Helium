/*
 * FiletypeControl.cpp
 *
 *  Created on: May 21, 2015
 *      Author: lingnan
 */

#include <bb/cascades/Header>
#include <bb/cascades/ToggleButton>
#include <bb/cascades/ToggleButton>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/Label>
#include <bb/cascades/SystemDefaults>
#include <FiletypeControl.h>
#include <Filetype.h>
#include <Segment.h>
#include <Utility.h>
#include <Defaults.h>

using namespace bb::cascades;

FiletypeControl::FiletypeControl(Filetype *filetype):
    _filetype(NULL),
    _highlightHeader(Header::create()),
    _highlightToggleLabel(Label::create()
        .layoutProperties(StackLayoutProperties::create().spaceQuota(1))
        .textStyle(SystemDefaults::TextStyles::primaryText())),
    _highlightToggle(ToggleButton::create()),
    _highlightToggleHelp(Label::create().multiline(true)
        .textStyle(Defaults::helpText())),
    _runProfileHeader(Header::create())
{
    add(_highlightHeader);
    add(Segment::create().subsection().leftToRight()
        .add(_highlightToggleLabel)
        .add(_highlightToggle));
    add(Segment::create().subsection()
        .add(_highlightToggleHelp));
    add(_runProfileHeader);

    onTranslatorChanged();
    setFiletype(filetype);
}

void FiletypeControl::setFiletype(Filetype *filetype)
{
    if (filetype != _filetype) {
        if (_filetype) {
            _highlightToggle->disconnect();
        }
        _filetype = filetype;
        reloadFiletypeHeader();
        if (_filetype) {
            _highlightToggle->setChecked(_filetype->highlightEnabled());
            conn(_highlightToggle, SIGNAL(checkedChanged(bool)),
                _filetype, SLOT(setHighlightEnabled(bool)));
        }
    }
}

void FiletypeControl::reloadFiletypeHeader()
{
    QString filetypeName;
    if (_filetype)
        filetypeName = _filetype->name();
    if (filetypeName.isEmpty())
        filetypeName = "Filetype";
    filetypeName[0] = filetypeName[0].toUpper();
    _highlightHeader->setTitle(tr("%1 Settings").arg(filetypeName));
}

void FiletypeControl::onTranslatorChanged()
{
    reloadFiletypeHeader();
    _runProfileHeader->setTitle(tr("Run Profile"));
    _highlightToggleLabel->setText(tr("Enable highlight"));
    _highlightToggleHelp->setText(tr("Highlight is automatically updated for a range of lines around the cursor"));
}
