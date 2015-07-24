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

GeneralSettingsPage::GeneralSettingsPage(GeneralSettings *generalSettings, QObject *parent):
    RepushablePage(parent),
    _settings(generalSettings),
    _highlightRangePicker(new NumberPicker(0, 40)),
    _highlightRangeHelp(Label::create().multiline(true)
        .textStyle(Defaults::helpText())),
    _chooseDefaultOpenDirOption(Option::create()
        .onSelectedChanged(this, SLOT(onChooseDefaultOpenDirSelectedChanged(bool)))),
    _currentDefaultOpenDirOption(Option::create()),
    _defaultOpenDirHelp(Label::create().multiline(true)
        .textStyle(Defaults::helpText())),
    _fpicker(NULL)
{
    setTitleBar(TitleBar::create());

    _highlightRangePicker->setSelectedNumber(_settings->highlightRange());
    conn(_highlightRangePicker, SIGNAL(selectedNumberChanged(int)),
        _settings, SLOT(setHighlightRange(int)));
    conn(_settings, SIGNAL(highlightRangeChanged(int)),
        _highlightRangePicker, SLOT(setSelectedNumber(int)));

    _defaultOpenDirSelect = DropDown::create()
        .add(_chooseDefaultOpenDirOption)
        .add(_currentDefaultOpenDirOption);
    resetDefaultOpenDirSelection();
    onDefaultOpenDirectoryChanged(_settings->defaultOpenDirectory());
    conn(_settings, SIGNAL(defaultOpenDirectoryChanged(const QString&)),
        this, SLOT(onDefaultOpenDirectoryChanged(const QString&)));

    setContent(ScrollView::create(Segment::create().section()
            .add(Segment::create().subsection().add(_highlightRangePicker))
            .add(Segment::create().subsection().add(_highlightRangeHelp))
            .add(Divider::create())
            .add(Segment::create().subsection().add(_defaultOpenDirSelect))
            .add(Segment::create().subsection().add(_defaultOpenDirHelp)))
        .scrollMode(ScrollMode::Vertical));

    onTranslatorChanged();
}

void GeneralSettingsPage::onChooseDefaultOpenDirSelectedChanged(bool selected)
{
    if (selected) {
        if (!_fpicker) {
            _fpicker = new pickers::FilePicker(this);
            conn(_fpicker, SIGNAL(fileSelected(const QStringList&)),
                this, SLOT(onDefaultOpenDirSelected(const QStringList&)));
            conn(_fpicker, SIGNAL(pickerClosed()),
                this, SLOT(resetDefaultOpenDirSelection()));
        }
        _fpicker->setMode(pickers::FilePickerMode::SaverMultiple);
        _fpicker->open();
    }
}

void GeneralSettingsPage::resetDefaultOpenDirSelection()
{
    _defaultOpenDirSelect->setSelectedOption(_currentDefaultOpenDirOption);
}

void GeneralSettingsPage::onDefaultOpenDirSelected(const QStringList &list)
{
    _settings->setDefaultOpenDirectory(list[0]);
}

void GeneralSettingsPage::onDefaultOpenDirectoryChanged(const QString &directory)
{
    _currentDefaultOpenDirOption->setText(directory);
}

void GeneralSettingsPage::onTranslatorChanged()
{
    titleBar()->setTitle(tr("General"));
    _highlightRangePicker->setTitle(tr("Highlight Range"));
    _highlightRangeHelp->setText(tr("Highlight range controls the number of lines to be highlighted on each side of the cursor. "
            "Adjusting this value down will improve performance and vice versa."));
    _defaultOpenDirSelect->setTitle(tr("Default Open Directory"));
    _chooseDefaultOpenDirOption->setText(tr("<Choose a directory>"));
    _defaultOpenDirHelp->setText(tr("The default directory the Open command uses when no file has been opened in the current buffer before"));
}
