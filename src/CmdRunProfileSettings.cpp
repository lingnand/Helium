/*
 * CmdRunProfileSettings.cpp
 *
 *  Created on: May 24, 2015
 *      Author: lingnan
 */

#include <bb/cascades/Header>
#include <bb/cascades/Label>
#include <bb/cascades/TextField>
#include <RunProfileSettings.h>
#include <CmdRunProfileSettings.h>
#include <CmdRunProfileManager.h>
#include <SignalBlocker.h>
#include <Segment.h>
#include <Defaults.h>
#include <Utility.h>

using namespace bb::cascades;

CmdRunProfileSettings::CmdRunProfileSettings(CmdRunProfileManager *manager):
    _header(Header::create()),
    _field(TextField::create().text(manager->cmd())),
    _help(Label::create().multiline(true)
        .textStyle(Defaults::helpText()))

{
    _field->input()->setSubmitKey(SubmitKey::Done);
    _field->input()->setSubmitKeyFocusBehavior(SubmitKeyFocusBehavior::Lose);
    conn(_field, SIGNAL(textChanging(const QString&)),
        manager, SLOT(setCmd(const QString&)));
    conn(manager, SIGNAL(cmdChanged(const QString&)),
        this, SLOT(onCmdChanged(const QString&)));
    add(_header);
    add(Segment::create().subsection().add(_field));
    add(Segment::create().subsection().add(_help));

    onTranslatorChanged();
}

void CmdRunProfileSettings::onCmdChanged(const QString &cmd)
{
    SignalBlocker blocker(_field);
    _field->setText(cmd);
}

void CmdRunProfileSettings::onTranslatorChanged()
{
    _header->setTitle(tr("Command Settings"));
    _help->setText(tr(
        "The command to pass to /bin/sh\n"
        "\n"
        "These format specifiers will be replaced:\n"
        "%1: the absolute path to the file\n"
        "%2: the directory of the file\n"
        "%3: the name of the file"));
}
