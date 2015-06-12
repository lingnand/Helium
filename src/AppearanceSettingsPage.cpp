/*
 * AppearanceSettingsPage.cpp
#include <bb/cascades/Label>
#include <bb/cascades/Label>
 *
 *  Created on: Jun 6, 2015
 *      Author: lingnan
 */

#include <bb/cascades/ToggleButton>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/TitleBar>
#include <bb/cascades/Label>
#include <bb/cascades/DropDown>
#include <bb/cascades/Divider>
#include <AppearanceSettingsPage.h>
#include <Segment.h>
#include <Utility.h>
#include <Defaults.h>

using namespace bb::cascades;

AppearanceSettingsPage::AppearanceSettingsPage(AppearanceSettings *appearanceSettings, QObject *parent):
    RepushablePage(parent),
    _settings(appearanceSettings),
    _hideActionBarToggleLabel(Label::create()
        .layoutProperties(StackLayoutProperties::create().spaceQuota(1))
        .textStyle(SystemDefaults::TextStyles::primaryText())),
    _hideActionBarToggle(ToggleButton::create()),
    _hideActionBarHelp(Label::create().multiline(true)
        .textStyle(Defaults::helpText())),
    _brightThemeOption(Option::create()
        .value(AppearanceSettings::Bright)),
    _darkThemeOption(Option::create()
        .value(AppearanceSettings::Dark)),
    _themeSelectHelp(Label::create().multiline(true)
        .textStyle(Defaults::helpText()))
{
    setTitleBar(TitleBar::create());

    _hideActionBarToggle->setChecked(appearanceSettings->hideActionBar());
    conn(_hideActionBarToggle, SIGNAL(checkedChanged(bool)),
        appearanceSettings, SLOT(setHideActionBar(bool)));
    conn(appearanceSettings, SIGNAL(hideActionBarChanged(bool)),
        _hideActionBarToggle, SLOT(setChecked(bool)));

    _themeSelect = DropDown::create()
        .add(_brightThemeOption)
        .add(_darkThemeOption);
    onThemeChanged(appearanceSettings->theme());
    conn(_themeSelect, SIGNAL(selectedValueChanged(const QVariant&)),
        this, SLOT(onThemeSelectionChanged(const QVariant&)));
    conn(appearanceSettings, SIGNAL(themeChanged(AppearanceSettings::Theme)),
        this, SLOT(onThemeChanged(AppearanceSettings::Theme)));

    setContent(Segment::create().section()
        .add(Segment::create().subsection().leftToRight()
            .add(_hideActionBarToggleLabel)
            .add(_hideActionBarToggle))
        .add(Segment::create().subsection().add(_hideActionBarHelp))
        .add(Divider::create())
        .add(Segment::create().subsection().add(_themeSelect))
        .add(Segment::create().subsection().add(_themeSelectHelp)));

    onTranslatorChanged();
}

void AppearanceSettingsPage::onThemeChanged(AppearanceSettings::Theme theme)
{
    switch (theme) {
        case AppearanceSettings::Bright:
            _themeSelect->setSelectedOption(_brightThemeOption); break;
        case AppearanceSettings::Dark:
            _themeSelect->setSelectedOption(_darkThemeOption); break;
    }
}

void AppearanceSettingsPage::onThemeSelectionChanged(const QVariant &v)
{
    _settings->setTheme((AppearanceSettings::Theme) v.toInt());
}

void AppearanceSettingsPage::onTranslatorChanged()
{
    titleBar()->setTitle(tr("Appearance"));
    _hideActionBarToggleLabel->setText(tr("Hide ActionBar"));
    _hideActionBarHelp->setText(tr("Hide the ActionBar under the text areas. All the actions are still accessible via keyboard shortcuts."));
    _themeSelect->setTitle(tr("Theme"));
    _brightThemeOption->setText(tr("Bright"));
    _darkThemeOption->setText(tr("Dark"));
    _themeSelectHelp->setText(tr("Overall theme for Helium"));
}
