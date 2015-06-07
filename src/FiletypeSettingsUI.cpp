/*
 * FiletypeSettingsUI.cpp
 *
 *  Created on: May 21, 2015
 *      Author: lingnan
 */

#include <bb/cascades/Header>
#include <bb/cascades/ToggleButton>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/Label>
#include <bb/cascades/DropDown>
#include <bb/cascades/SystemDefaults>
#include <FiletypeSettingsUI.h>
#include <Filetype.h>
#include <RunProfileManager.h>
#include <RunProfileSettingsUI.h>
#include <Segment.h>
#include <Utility.h>
#include <Defaults.h>
#include <SignalBlocker.h>

using namespace bb::cascades;

FiletypeSettingsUI::FiletypeSettingsUI(Filetype *filetype):
    _filetype(NULL),
    _header(Header::create()),
    _highlightToggleLabel(Label::create()
        .layoutProperties(StackLayoutProperties::create().spaceQuota(1))
        .textStyle(SystemDefaults::TextStyles::primaryText())),
    _highlightToggle(ToggleButton::create()),
    _highlightToggleHelp(Label::create().multiline(true)
        .textStyle(Defaults::helpText())),
    _runProfileHeader(Header::create()),
    _noneRunProfileOption(Option::create()
            .value(RunProfileManager::None)),
    _cmdRunProfileOption(Option::create()
            .value(RunProfileManager::Cmd)),
    _webRunProfileOption(Option::create()
            .value(RunProfileManager::Web)),
    _runProfileSettingsUI(NULL)
{
    conn(_highlightToggle, SIGNAL(checkedChanged(bool)),
        this, SLOT(onHighlightCheckedChanged(bool)));

    _runProfileSelect = DropDown::create()
        .add(_noneRunProfileOption)
        .add(_cmdRunProfileOption)
        .add(_webRunProfileOption);
    conn(_runProfileSelect, SIGNAL(selectedValueChanged(const QVariant)),
        this, SLOT(onRunProfileSelectionChanged(const QVariant)));

    add(_header);
    add(Segment::create().subsection().leftToRight()
        .add(_highlightToggleLabel)
        .add(_highlightToggle));
    add(Segment::create().subsection().add(_highlightToggleHelp));
    add(_runProfileHeader);
    add(Segment::create().subsection().add(_runProfileSelect));

    onTranslatorChanged();
    setFiletype(filetype);
}

void FiletypeSettingsUI::setFiletype(Filetype *filetype)
{
    if (filetype != _filetype) {
        if (_filetype) {
            _filetype->disconnect(this);
            _filetype->disconnect(_highlightToggle);
        }
        _filetype = filetype;
        reloadHeader();
        _highlightToggle->setChecked(_filetype ?
                _filetype->highlightEnabled() :
                false);
        onFiletypeRunProfileManagerChanged(_filetype ?
                _filetype->runProfileManager() :
                NULL);
        if (_filetype) {
            conn(_filetype, SIGNAL(highlightEnabledChanged(bool)),
                _highlightToggle, SLOT(setChecked(bool)));
            conn(_filetype, SIGNAL(runProfileManagerChanged(RunProfileManager*, RunProfileManager*)),
                this, SLOT(onFiletypeRunProfileManagerChanged(RunProfileManager*)));
        }
    }
}

void FiletypeSettingsUI::onFiletypeRunProfileManagerChanged(RunProfileManager *change)
{
    SignalBlocker blocker(_runProfileSelect);
    switch (RunProfileManager::type(change)) {
        case RunProfileManager::Cmd:
            _runProfileSelect->setSelectedOption(_cmdRunProfileOption); break;
        case RunProfileManager::Web:
            _runProfileSelect->setSelectedOption(_webRunProfileOption); break;
        default:
            _runProfileSelect->setSelectedOption(_noneRunProfileOption);
    }
    if (_runProfileSettingsUI) {
        remove(_runProfileSettingsUI);
        _runProfileSettingsUI->deleteLater();
        _runProfileSettingsUI = NULL;
    }
    if (change) {
        add(_runProfileSettingsUI = RunProfileSettingsUI::create(change));
    }
}

void FiletypeSettingsUI::reloadHeader()
{
    QString filetypeName;
    if (_filetype)
        filetypeName = _filetype->name();
    if (filetypeName.isEmpty())
        filetypeName = "Filetype";
    filetypeName[0] = filetypeName[0].toUpper();
    _header->setTitle(tr("%1 Settings").arg(filetypeName));
}

void FiletypeSettingsUI::onHighlightCheckedChanged(bool checked)
{
    if (_filetype) {
        _filetype->setHighlightEnabled(checked);
    }
}

void FiletypeSettingsUI::onRunProfileSelectionChanged(const QVariant v)
{
    if (_filetype) {
        _filetype->setRunProfileManager(
                RunProfileManager::create((RunProfileManager::Type) v.toInt()));
    }
}

void FiletypeSettingsUI::onTranslatorChanged()
{
    reloadHeader();
    _highlightToggleLabel->setText(tr("Enable highlight"));
    _highlightToggleHelp->setText(tr("Highlight is automatically updated for a range of lines around the cursor"));
    _runProfileHeader->setTitle(tr("Run Profile"));
    _runProfileSelect->setTitle("Run Profile");
    _noneRunProfileOption->setDescription(tr("<No Run Profile>"));
    _cmdRunProfileOption->setText(tr("Run Command"));
    _cmdRunProfileOption->setDescription(tr("Pass a command to /bin/sh"));
    _webRunProfileOption->setText(tr("Web View"));
    _webRunProfileOption->setDescription(tr("Preview in web page"));
    if (_runProfileSettingsUI) {
        _runProfileSettingsUI->onTranslatorChanged();
    }
}
