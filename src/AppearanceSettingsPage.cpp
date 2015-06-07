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
#include <AppearanceSettingsPage.h>
#include <AppearanceSettings.h>
#include <Segment.h>
#include <Utility.h>
#include <Defaults.h>

using namespace bb::cascades;

AppearanceSettingsPage::AppearanceSettingsPage(AppearanceSettings *appearanceSettings, QObject *parent):
    RepushablePage(parent),
    _hideActionBarToggleLabel(Label::create()
        .layoutProperties(StackLayoutProperties::create().spaceQuota(1))
        .textStyle(SystemDefaults::TextStyles::primaryText())),
    _hideActionBarToggle(ToggleButton::create()),
    _hideActionBarHelp(Label::create().multiline(true)
        .textStyle(Defaults::helpText()))
{
    setTitleBar(TitleBar::create());

    _hideActionBarToggle->setChecked(appearanceSettings->hideActionBar());
    conn(_hideActionBarToggle, SIGNAL(checkedChanged(bool)),
        appearanceSettings, SLOT(setHideActionBar(bool)));
    conn(appearanceSettings, SIGNAL(hideActionBarChanged(bool)),
        _hideActionBarToggle, SLOT(setChecked(bool)));

    setContent(Segment::create().section()
        .add(Segment::create().subsection().leftToRight()
            .add(_hideActionBarToggleLabel)
            .add(_hideActionBarToggle))
        .add(Segment::create().subsection().add(_hideActionBarHelp)));

    onTranslatorChanged();
}

void AppearanceSettingsPage::onTranslatorChanged()
{
    titleBar()->setTitle(tr("Appearance"));
    _hideActionBarToggleLabel->setText(tr("Hide ActionBar"));
    _hideActionBarHelp->setText(tr("Hide the ActionBar under the text areas. All the actions are still accessible via keyboard shortcuts."));
}

