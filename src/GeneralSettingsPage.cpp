/*
 * GeneralSettingsPage.cpp
 *
 *  Created on: May 25, 2015
 *      Author: lingnan
 */

#include <bb/cascades/TitleBar>
#include <bb/cascades/Picker>
#include <bb/cascades/Label>
#include <bb/cascades/DropDown>
#include <bb/cascades/Option>
#include <bb/cascades/Divider>
#include <bb/cascades/ScrollView>
#include <bb/cascades/pickers/FilePicker>
#include <GeneralSettingsPage.h>
#include <GeneralSettings.h>
#include <NumberPicker.h>
#include <Segment.h>
#include <Utility.h>
#include <Defaults.h>

using namespace bb::cascades;

GeneralSettingsPage::GeneralSettingsPage(GeneralSettings *generalSettings):
    _settings(generalSettings),
    _highlightRangePicker(new NumberPicker(0, 40)),
    _highlightRangeHelp(Label::create().multiline(true)
        .textStyle(Defaults::helpText())),
    _chooseDefaultProjectDirOption(Option::create()),
    _currentDefaultProjectDirOption(Option::create()),
    _defaultProjectDirSelect(DropDown::create()
        .add(_chooseDefaultProjectDirOption)
        .add(_currentDefaultProjectDirOption)),
    _defaultProjectDirHelp(Label::create().multiline(true)
        .textStyle(Defaults::helpText())),
    _fpicker(NULL)
{
    setTitleBar(TitleBar::create());

    _highlightRangePicker->setSelectedNumber(_settings->highlightRange());
    conn(_highlightRangePicker, SIGNAL(selectedNumberChanged(int)),
        _settings, SLOT(setHighlightRange(int)));
    conn(_settings, SIGNAL(highlightRangeChanged(int)),
        _highlightRangePicker, SLOT(setSelectedNumber(int)));

    resetDefaultProjectDirSelection();
    conn(_chooseDefaultProjectDirOption, SIGNAL(selectedChanged(bool)),
        this, SLOT(onChooseDefaultProjectDirSelectedChanged(bool)));
    onDefaultProjectDirectoryChanged(_settings->defaultProjectDirectory());
    conn(_settings, SIGNAL(defaultProjectDirectoryChanged(const QString&)),
        this, SLOT(onDefaultProjectDirectoryChanged(const QString&)));

    setContent(ScrollView::create(Segment::create().section()
            .add(Segment::create().subsection().add(_highlightRangePicker))
            .add(Segment::create().subsection().add(_highlightRangeHelp))
            .add(Divider::create())
            .add(Segment::create().subsection().add(_defaultProjectDirSelect))
            .add(Segment::create().subsection().add(_defaultProjectDirHelp)))
        .scrollMode(ScrollMode::Vertical));

    onTranslatorChanged();
}

void GeneralSettingsPage::onChooseDefaultProjectDirSelectedChanged(bool selected)
{
    if (selected) {
        if (!_fpicker) {
            _fpicker = new pickers::FilePicker(this);
            _fpicker->setMode(pickers::FilePickerMode::SaverMultiple);
            conn(_fpicker, SIGNAL(fileSelected(const QStringList&)),
                this, SLOT(onDefaultProjectDirSelected(const QStringList&)));
            conn(_fpicker, SIGNAL(pickerClosed()),
                this, SLOT(resetDefaultProjectDirSelection()));
        }
        _fpicker->open();
    }
}

void GeneralSettingsPage::resetDefaultProjectDirSelection()
{
    _defaultProjectDirSelect->setSelectedOption(_currentDefaultProjectDirOption);
}

void GeneralSettingsPage::onDefaultProjectDirSelected(const QStringList &list)
{
    _settings->setDefaultProjectDirectory(list[0]);
}

void GeneralSettingsPage::onDefaultProjectDirectoryChanged(const QString &directory)
{
    _currentDefaultProjectDirOption->setText(directory);
}

void GeneralSettingsPage::onTranslatorChanged()
{
    PushablePage::onTranslatorChanged();
    titleBar()->setTitle(tr("General"));
    _highlightRangePicker->setTitle(tr("Highlight Range"));
    _highlightRangeHelp->setText(tr("Highlight range controls the number of lines to be highlighted on each side of the cursor. "
            "Adjusting this value down will improve performance and vice versa."));
    _defaultProjectDirSelect->setTitle(tr("Default Project Directory"));
    _chooseDefaultProjectDirOption->setText(tr("<Choose a Directory>"));
    _defaultProjectDirHelp->setText(tr("The default directory for a new project"));
}
