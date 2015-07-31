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
#include <bb/cascades/Divider>
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
    _tabSpaceConversionToggleLabel(Label::create()
        .layoutProperties(StackLayoutProperties::create().spaceQuota(1))
        .textStyle(SystemDefaults::TextStyles::primaryText())),
    _tabSpaceConversionToggle(ToggleButton::create()),
    _tabSpaceConversionToggleHelp(Label::create().multiline(true)
        .textStyle(Defaults::helpText())),
    _numberOfSpacesForTabSelect(DropDown::create()
        .enabled(_tabSpaceConversionToggle->isChecked())
        .add("2", 2)
        .add("4", 4)
        .add("8", 8)),
    _numberOfSpacesForTabHelp(Label::create().multiline(true)
        .textStyle(Defaults::helpText())),
    _runProfileHeader(Header::create()),
    _noneRunProfileOption(Option::create()
            .value(RunProfileManager::None)),
    _cmdRunProfileOption(Option::create()
            .value(RunProfileManager::Cmd)),
    _webRunProfileOption(Option::create()
            .value(RunProfileManager::Web)),
    _runProfileSelect(DropDown::create()
        .add(_noneRunProfileOption)
        .add(_cmdRunProfileOption)
        .add(_webRunProfileOption)),
    _runProfileSettingsUI(NULL)
{
    conn(_tabSpaceConversionToggle, SIGNAL(checkedChanged(bool)),
        _numberOfSpacesForTabSelect, SLOT(setEnabled(bool)));

    conn(_runProfileSelect, SIGNAL(selectedValueChanged(const QVariant&)),
        this, SLOT(onRunProfileSelectionChanged(const QVariant&)));

    conn(_numberOfSpacesForTabSelect, SIGNAL(selectedValueChanged(const QVariant&)),
        this, SLOT(onNumberOfSpacesForTabSelectionChanged(const QVariant &)));

    add(_header);
    add(Segment::create().subsection().leftToRight()
        .add(_highlightToggleLabel)
        .add(_highlightToggle));
    add(Segment::create().subsection().add(_highlightToggleHelp));
    add(Divider::create());
    add(Segment::create().subsection().leftToRight()
        .add(_tabSpaceConversionToggleLabel)
        .add(_tabSpaceConversionToggle));
    add(Segment::create().subsection().add(_tabSpaceConversionToggleHelp));
    add(Segment::create().subsection().add(_numberOfSpacesForTabSelect));
    add(Segment::create().subsection().add(_numberOfSpacesForTabHelp));
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
            _filetype->disconnect(_tabSpaceConversionToggle);
            _highlightToggle->disconnect(_filetype);
            _tabSpaceConversionToggle->disconnect(_filetype);
        }
        _filetype = filetype;
        reloadHeader();
        _highlightToggle->setChecked(_filetype ?
                _filetype->highlightEnabled() :
                false);
        _tabSpaceConversionToggle->setChecked(_filetype ?
                _filetype->tabSpaceConversionEnabled() :
                false);
        onFiletypeNumberOfSpacesForTabChanged(_filetype ?
                _filetype->numberOfSpacesForTab() :
                0);
        onFiletypeRunProfileManagerChanged(_filetype ?
                _filetype->runProfileManager() :
                NULL);
        if (_filetype) {
            conn(_filetype, SIGNAL(highlightEnabledChanged(bool)),
                _highlightToggle, SLOT(setChecked(bool)));
            conn(_highlightToggle, SIGNAL(checkedChanged(bool)),
                _filetype, SLOT(setHighlightEnabled(bool)));
            conn(_filetype, SIGNAL(tabSpaceConversionEnabledChanged(bool)),
                _tabSpaceConversionToggle, SLOT(setChecked(bool)));
            conn(_tabSpaceConversionToggle, SIGNAL(checkedChanged(bool)),
                _filetype, SLOT(setTabSpaceConversionEnabled(bool)));
            conn(_filetype, SIGNAL(numberOfSpacesForTabChanged(int)),
                this, SLOT(onFiletypeNumberOfSpacesForTabChanged(int)));
            conn(_filetype, SIGNAL(runProfileManagerChanged(RunProfileManager*, RunProfileManager*)),
                this, SLOT(onFiletypeRunProfileManagerChanged(RunProfileManager*)));
        }
    }
}

void FiletypeSettingsUI::onFiletypeRunProfileManagerChanged(RunProfileManager *change)
{
    SignalBlocker blocker(_runProfileSelect);
    RunProfileManager::Type type = RunProfileManager::type(change);
    for (int i = 0; i < _runProfileSelect->count(); i++) {
        if (_runProfileSelect->at(i)->value().toInt() == type) {
            _runProfileSelect->setSelectedIndex(i);
            break;
        }
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

void FiletypeSettingsUI::onFiletypeNumberOfSpacesForTabChanged(int number)
{
    SignalBlocker blocker(_numberOfSpacesForTabSelect);
    for (int i = 0; i < _numberOfSpacesForTabSelect->count(); i++) {
        if (_numberOfSpacesForTabSelect->at(i)->value().toInt() == number) {
            _numberOfSpacesForTabSelect->setSelectedIndex(i);
            return;
        }
    }
    // default
    _numberOfSpacesForTabSelect->setSelectedIndex(4);
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

void FiletypeSettingsUI::onRunProfileSelectionChanged(const QVariant &v)
{
    if (_filetype) {
        _filetype->setRunProfileManager(
                RunProfileManager::create((RunProfileManager::Type) v.toInt()));
    }
}

void FiletypeSettingsUI::onNumberOfSpacesForTabSelectionChanged(const QVariant &v)
{
    if (_filetype) {
        _filetype->setNumberOfSpacesForTab(v.toInt());
    }
}

void FiletypeSettingsUI::onTranslatorChanged()
{
    reloadHeader();
    _highlightToggleLabel->setText(tr("Enable Highlight"));
    _highlightToggleHelp->setText(tr("Highlight is automatically updated for a range of lines around the cursor"));
    _tabSpaceConversionToggleLabel->setText(tr("Enable Tab-Space Conversion"));
    _tabSpaceConversionToggleHelp->setText(tr("Space indent will be displayed as tabs in the buffer on file load; the same tabs will be converted to spaces on file save"));
    _numberOfSpacesForTabSelect->setTitle(tr("Number of Spaces Per Tab"));
    _numberOfSpacesForTabHelp->setText(tr("The number of spaces for each tab on conversion"));
    _runProfileHeader->setTitle(tr("Run Profile"));
    _runProfileSelect->setTitle(tr("Run Profile"));
    _noneRunProfileOption->setDescription(tr("<No Run Profile>"));
    _cmdRunProfileOption->setText(tr("Run Command"));
    _cmdRunProfileOption->setDescription(tr("Pass a command to /bin/sh"));
    _webRunProfileOption->setText(tr("Web View"));
    _webRunProfileOption->setDescription(tr("Preview in web page"));
    if (_runProfileSettingsUI) {
        _runProfileSettingsUI->onTranslatorChanged();
    }
}
