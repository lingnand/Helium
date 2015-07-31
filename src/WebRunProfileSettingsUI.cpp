/*
 * WebRunProfileSettingsUI.cpp
 *
 *  Created on: Jun 4, 2015
 *      Author: lingnan
 */

#include <bb/cascades/Header>
#include <bb/cascades/Label>
#include <bb/cascades/DropDown>
#include <WebRunProfileSettingsUI.h>
#include <WebRunProfileManager.h>
#include <SignalBlocker.h>
#include <Segment.h>
#include <Utility.h>
#include <Defaults.h>

using namespace bb::cascades;

WebRunProfileSettingsUI::WebRunProfileSettingsUI(WebRunProfileManager *manager):
    _manager(manager),
    _header(Header::create()),
    _htmlOption(Option::create()
        .value(WebRunProfile::Html)),
    _javascriptOption(Option::create()
        .value(WebRunProfile::Javascript)),
    _markdownOption(Option::create()
        .value(WebRunProfile::Markdown)),
    _modeSelect(DropDown::create()
        .add(_htmlOption)
        .add(_javascriptOption)
        .add(_markdownOption)),
    _help(Label::create()
        .textStyle(Defaults::helpText()))
{
    conn(_modeSelect, SIGNAL(selectedValueChanged(const QVariant&)),
            this, SLOT(onModeSelectionChanged(const QVariant&)));
    onManagerModeChanged(manager->mode());
    conn(manager, SIGNAL(modeChanged(WebRunProfile::Mode)),
            this, SLOT(onManagerModeChanged(WebRunProfile::Mode)));

    add(_header);
    add(Segment::create().subsection().add(_modeSelect));
    add(Segment::create().subsection().add(_help));

    onTranslatorChanged();
}

void WebRunProfileSettingsUI::onModeSelectionChanged(const QVariant &v)
{
    _manager->setMode((WebRunProfile::Mode) v.toInt());
}

void WebRunProfileSettingsUI::onManagerModeChanged(WebRunProfile::Mode mode)
{
    SignalBlocker blocker(_modeSelect);
    switch (mode) {
        case WebRunProfile::Html:
            _modeSelect->setSelectedOption(_htmlOption);
            break;
        case WebRunProfile::Javascript:
            _modeSelect->setSelectedOption(_javascriptOption);
            break;
        case WebRunProfile::Markdown:
            _modeSelect->setSelectedOption(_markdownOption);
            break;
    }
}

void WebRunProfileSettingsUI::onTranslatorChanged()
{
    _header->setTitle(tr("Mode Settings"));
    _modeSelect->setTitle(tr("Evaluation Mode"));
    _htmlOption->setText(tr("Html"));
    _htmlOption->setDescription(tr("text in buffer is used as html"));
    _javascriptOption->setText(tr("Javascript"));
    _javascriptOption->setDescription(tr("text in buffer is injected as javascript into the page"));
    _markdownOption->setText(tr("Markdown"));
    _markdownOption->setDescription(tr("text in buffer is converted from markdown to html"));
    _help->setText(tr("The evaluation mode for the web view."));
}
