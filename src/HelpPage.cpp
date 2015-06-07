/*
 * HelpPage.cpp
 *
 *  Created on: Jun 7, 2015
 *      Author: lingnan
 */

#include <bb/cascades/TitleBar>
#include <bb/cascades/Option>
#include <bb/cascades/TextArea>
#include <HelpPage.h>

using namespace bb::cascades;

HelpPage::HelpPage(QObject *parent):
    RepushablePage(parent),
    _quickStartOption(Option::create()),
    _keyboardShortcutsOption(Option::create()),
    _runProfileOption(Option::create()),
    _textArea(TextArea::create()
        .format(TextFormat::Html)
        .contentFlags(TextContentFlag::ActiveTextOff)
        .editable(false))
{
    setTitleBar(TitleBar::create(TitleBarKind::Segmented)
        .addOption(_quickStartOption)
        .addOption(_keyboardShortcutsOption)
        .addOption(_runProfileOption));
    setContent(_textArea);

    onTranslatorChanged();
}

void HelpPage::onTranslatorChanged()
{
    _quickStartOption->setText(tr("Quick Start"));
    _keyboardShortcutsOption->setText(tr("Keyboard Shortcuts"));
    _runProfileOption->setText(tr("Run Profiles"));
}

