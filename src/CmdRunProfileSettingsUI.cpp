/*
 * CmdRunProfileSettingsUI.cpp
 *
 *  Created on: May 24, 2015
 *      Author: lingnan
 */

#include <bb/cascades/Header>
#include <bb/cascades/Label>
#include <bb/cascades/TextField>
#include <RunProfileSettingsUI.h>
#include <CmdRunProfileSettingsUI.h>
#include <CmdRunProfileManager.h>
#include <SignalBlocker.h>
#include <Segment.h>
#include <Defaults.h>
#include <Utility.h>

using namespace bb::cascades;

CmdRunProfileSettingsUI::CmdRunProfileSettingsUI(CmdRunProfileManager *manager):
    _header(Header::create()),
    _field(TextField::create()
        .inputFlags(Defaults::codeInputFlags())
        .contentFlags(Defaults::codeContentFlags())
        .text(manager->cmd())),
    _help(Label::create().multiline(true)
        .textStyle(Defaults::helpText()))

{
    _field->input()->setSubmitKey(SubmitKey::Done);
    _field->input()->setSubmitKeyFocusBehavior(SubmitKeyFocusBehavior::Lose);
    conn(_field, SIGNAL(textChanged(const QString&)),
        manager, SLOT(setCmd(const QString&)));
    conn(manager, SIGNAL(cmdChanged(const QString&)),
        _field, SLOT(setText(const QString&)));
    add(_header);
    add(Segment::create().subsection().add(_field));
    add(Segment::create().subsection().add(_help));

    onTranslatorChanged();
}

void CmdRunProfileSettingsUI::onTranslatorChanged()
{
    _header->setTitle(tr("Command Settings"));
    _field->setHintText(tr("Command"));
    _help->setText(tr(
        "The command to pass to /bin/sh\n"
        "\n"
        "These format specifiers will be replaced:\n"
        "%path%: the absolute path to the file\n"
        "%dir%: the directory of the file\n"
        "%name%: the name of the file\n"
        "%%: a literal %"));
}
