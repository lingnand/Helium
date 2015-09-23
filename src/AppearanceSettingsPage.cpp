/*
 * AppearanceSettingsPage.cpp
 *
 *  Created on: Jun 6, 2015
 *      Author: lingnan
 */

#include <bb/cascades/Header>
#include <bb/cascades/ToggleButton>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/TitleBar>
#include <bb/cascades/Label>
#include <bb/cascades/DropDown>
#include <bb/cascades/Divider>
#include <bb/cascades/ScrollView>
#include <AppearanceSettingsPage.h>
#include <Segment.h>
#include <Utility.h>
#include <Defaults.h>
#include <NumberPicker.h>

using namespace bb::cascades;

AppearanceSettingsPage::AppearanceSettingsPage(AppearanceSettings *appearanceSettings):
    _settings(appearanceSettings),
    _themeSelect(DropDown::create()
        .add("Summer Fruit", AppearanceSettings::SummerFruit)
        .add("JellyX", AppearanceSettings::JellyX)
        .add("Tomorrow", AppearanceSettings::Tomorrow)
        .add("Tomorrow Night", AppearanceSettings::TomorrowNight)),
    _themeSelectHelp(Label::create().multiline(true)
        .textStyle(Defaults::helpText())),
    _fontHeader(Header::create()),
    _defaultFontFamilyOption(Option::create().value(QString())),
    _fontFamilySelect(DropDown::create()
        .add(_defaultFontFamilyOption)
        .add("Andale Mono", "Andale Mono")
        .add("Arial", "Arial")
        .add("Arial Black", "Arial Black")
        .add("Courier New", "Courier New")
        .add("DejaVu Sans", "DejaVu Sans")
        .add("DejaVu Sans Condensed", "DejaVu Sans Condensed")
        .add("DejaVu Sans Mono", "DejaVu Sans Mono")
        .add("DejaVu Serif", "DejaVu Serif")
        .add("DejaVu Serif Condensed", "DejaVu Serif Condensed")
        .add("Georgia", "Georgia")
        .add("Impact", "Impact")
        .add("MT Extra", "MT Extra")
        .add("Slate Pro", "Slate Pro")
        .add("Symbol", "Symbol")
        .add("Tahoma", "Tahoma")
        .add("Times New Roman", "Times New Roman")
        .add("Trebuchet MS", "Trebuchet MS")
        .add("Verdana", "Verdana")
        .add("Webdings", "Webdings")
        .add("Webdings 2", "Webdings 2")
        .add("Webdings 3", "Webdings 3")),
    _defaultFontSizeOption(Option::create().value(FontSize::Default)),
    _extraSmallFontSizeOption(Option::create().value(FontSize::XSmall)),
    _smallFontSizeOption(Option::create().value(FontSize::Small)),
    _mediumFontSizeOption(Option::create().value(FontSize::Medium)),
    _largeFontSizeOption(Option::create().value(FontSize::Large)),
    _extraLargeFontSizeOption(Option::create().value(FontSize::XLarge)),
    _fontSizeSelect(DropDown::create()
        .add(_defaultFontSizeOption)
        .add(_extraSmallFontSizeOption)
        .add(_smallFontSizeOption)
        .add(_mediumFontSizeOption)
        .add(_largeFontSizeOption)
        .add(_extraLargeFontSizeOption)),
    _fullScreenHeader(Header::create()),
    _hideActionBarToggleLabel(Label::create()
        .layoutProperties(StackLayoutProperties::create().spaceQuota(1))
        .textStyle(SystemDefaults::TextStyles::primaryText())),
    _hideActionBarToggle(ToggleButton::create()),
    _hideTitleBarToggleLabel(Label::create()
        .layoutProperties(StackLayoutProperties::create().spaceQuota(1))
        .textStyle(SystemDefaults::TextStyles::primaryText())),
    _hideTitleBarToggle(ToggleButton::create()),
    _fullScreenHelp(Label::create().multiline(true)
        .textStyle(Defaults::helpText()))
{
    setTitleBar(TitleBar::create());

    _hideActionBarToggle->setChecked(appearanceSettings->hideActionBar());
    conn(_hideActionBarToggle, SIGNAL(checkedChanged(bool)),
        appearanceSettings, SLOT(setHideActionBar(bool)));
    conn(appearanceSettings, SIGNAL(hideActionBarChanged(bool)),
        _hideActionBarToggle, SLOT(setChecked(bool)));
    _hideTitleBarToggle->setChecked(appearanceSettings->hideTitleBar());
    conn(_hideTitleBarToggle, SIGNAL(checkedChanged(bool)),
        appearanceSettings, SLOT(setHideTitleBar(bool)));
    conn(appearanceSettings, SIGNAL(hideTitleBarChanged(bool)),
        _hideTitleBarToggle, SLOT(setChecked(bool)));

    onThemeChanged(appearanceSettings->theme());
    conn(_themeSelect, SIGNAL(selectedValueChanged(const QVariant&)),
        this, SLOT(onThemeSelectionChanged(const QVariant&)));
    conn(appearanceSettings, SIGNAL(themeChanged(AppearanceSettings::Theme)),
        this, SLOT(onThemeChanged(AppearanceSettings::Theme)));

    onFontFamilyChanged(appearanceSettings->fontFamily());
    conn(_fontFamilySelect, SIGNAL(selectedValueChanged(const QVariant&)),
        this, SLOT(onFontFamilySelectionChanged(const QVariant&)));
    conn(appearanceSettings, SIGNAL(fontFamilyChanged(const QString&)),
        this, SLOT(onFontFamilyChanged(const QString&)));

    onFontSizeChanged(appearanceSettings->fontSize());
    conn(_fontSizeSelect, SIGNAL(selectedValueChanged(const QVariant&)),
        this, SLOT(onFontSizeSelectionChanged(const QVariant&)));
    conn(appearanceSettings, SIGNAL(fontSizeChanged(bb::cascades::FontSize::Type)),
        this, SLOT(onFontSizeChanged(bb::cascades::FontSize::Type)));

    setContent(ScrollView::create(Segment::create().section()
            .add(Segment::create().subsection().add(_themeSelect))
            .add(Segment::create().subsection().add(_themeSelectHelp))
            .add(_fontHeader)
            .add(Segment::create().subsection().add(_fontFamilySelect))
            .add(Segment::create().subsection().add(_fontSizeSelect))
            .add(_fullScreenHeader)
            .add(Segment::create().subsection().leftToRight()
                .add(_hideActionBarToggleLabel)
                .add(_hideActionBarToggle))
            .add(Segment::create().subsection().leftToRight()
                .add(_hideTitleBarToggleLabel)
                .add(_hideTitleBarToggle))
            .add(Segment::create().subsection().add(_fullScreenHelp)))
        .scrollMode(ScrollMode::Vertical));

    onTranslatorChanged();
}

void AppearanceSettingsPage::onThemeChanged(AppearanceSettings::Theme theme)
{
    for (int i = 0; i < _themeSelect->count(); i++) {
        if (_themeSelect->at(i)->value().toInt() == theme) {
            _themeSelect->setSelectedIndex(i);
            return;
        }
    }
}

void AppearanceSettingsPage::onThemeSelectionChanged(const QVariant &v)
{
    _settings->setTheme((AppearanceSettings::Theme) v.toInt());
}

void AppearanceSettingsPage::onFontFamilyChanged(const QString &fontFamily)
{
    for (int i = 0; i < _fontFamilySelect->count(); i++) {
        if (_fontFamilySelect->at(i)->value().toString() == fontFamily) {
            _fontFamilySelect->setSelectedIndex(i);
            return;
        }
    }
}

void AppearanceSettingsPage::onFontFamilySelectionChanged(const QVariant &v)
{
    _settings->setFontFamily(v.toString());
}

void AppearanceSettingsPage::onFontSizeChanged(bb::cascades::FontSize::Type size)
{
    for (int i = 0; i < _fontSizeSelect->count(); i++) {
        if (_fontSizeSelect->at(i)->value().toInt() == size) {
            _fontSizeSelect->setSelectedIndex(i);
            return;
        }
    }
}

void AppearanceSettingsPage::onFontSizeSelectionChanged(const QVariant &v)
{
    _settings->setFontSize((bb::cascades::FontSize::Type) v.toInt());
}

void AppearanceSettingsPage::onTranslatorChanged()
{
    PushablePage::onTranslatorChanged();
    titleBar()->setTitle(tr("Appearance Settings"));
    _themeSelect->setTitle(tr("Theme"));
    _themeSelectHelp->setText(tr("Overall theme for Helium"));
    _fontHeader->setTitle(tr("Font Settings"));
    _fontFamilySelect->setTitle(tr("Font Family"));
    _defaultFontFamilyOption->setText(tr("Default"));
    _fontSizeSelect->setTitle(tr("Font Size"));
    _defaultFontSizeOption->setText(tr("Default"));
    _extraSmallFontSizeOption->setText(tr("Extra Small"));
    _smallFontSizeOption->setText(tr("Small"));
    _mediumFontSizeOption->setText(tr("Medium"));
    _largeFontSizeOption->setText(tr("Large"));
    _extraLargeFontSizeOption->setText(tr("Extra Large"));
    _fullScreenHeader->setTitle(tr("Full-screen Settings"));
    _hideActionBarToggleLabel->setText(tr("Hide ActionBar"));
    _hideTitleBarToggleLabel->setText(tr("Hide TitleBar"));
    _fullScreenHelp->setText(tr("These settings affect the UI inside Full-screen mode. Full-screen mode can be toggled by sliding down from the top of the screen and selecting Enter/Exit Full-screen."));
}