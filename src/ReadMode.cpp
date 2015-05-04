/*
 * ReadMode.cpp
 *
 *  Created on: May 4, 2015
 *      Author: lingnan
 */

#include <bb/cascades/ActionItem>
#include <bb/cascades/Shortcut>
#include <bb/cascades/Page>
#include <bb/cascades/TextField>
#include <bb/cascades/TextArea>
#include <View.h>
#include <Buffer.h>
#include <ModKeyListener.h>
#include <ReadMode.h>
#include <NormalMode.h>

using namespace bb::cascades;

ReadMode::ReadMode(NormalMode *mode): ViewMode(mode->view()),
        _norm(mode), _editAction(NULL) {}

void ReadMode::autoFocus(bool)
{}

void ReadMode::onEnter(bool hasPreviousMode)
{
    view()->content()->removeAllActions();
    _editAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_edit.png"))
        .addShortcut(Shortcut::create().key("e"))
        .onTriggered(view(), SLOT(setNormalMode()));
//    _findAction = ActionItem::create()
//        .imageSource(QUrl("asset:///images/ic_search.png"))
//        .addShortcut(Shortcut::create().key("f"))
//        .onTriggered(view(), SLOT(setFindMode()));
    _openAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_open.png"))
        .addShortcut(Shortcut::create().key("e"))
        .onTriggered(view(), SLOT(open()));
    _cloneAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_copy_link.png"))
        .addShortcut(Shortcut::create().key("y"))
        .onTriggered(view(), SLOT(clone()));
    _closeAction = ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_clear.png"))
        .addShortcut(Shortcut::create().key("x"))
        .onTriggered(view(), SLOT(close()));
    reloadActionTitles();

    setLocked(view()->buffer()->locked());
    conn(view(), SIGNAL(bufferLockedChanged(bool)), this, SLOT(setLocked(bool)));

    view()->content()->addAction(_editAction, ActionBarPlacement::Signature);
    view()->content()->addAction(_openAction, ActionBarPlacement::OnBar);
//    view()->content()->addAction(_findAction, ActionBarPlacement::OnBar);
    view()->content()->addAction(_cloneAction);
    view()->content()->addAction(_closeAction);
    view()->content()->setTitleBar(_norm->titleBar());

    _norm->titleField()->setEnabled(false);
    view()->textArea()->setEditable(false);
    view()->textAreaModKeyListener()->setEnabled(false);
    view()->setHighlightRangeLimit(INT_MAX);
}

void ReadMode::onExit()
{
    disconn(view(), SIGNAL(bufferLockedChanged(bool)), this, SLOT(setLocked(bool)));
    _editAction = NULL;
}

void ReadMode::setLocked(bool locked)
{
    if (_editAction) {
        _editAction->setEnabled(!locked);
        _openAction->setEnabled(!locked);
//        _findAction->setEnabled(!locked);
        _cloneAction->setEnabled(!locked);
        _closeAction->setEnabled(!locked);
    }
}

void ReadMode::onTranslatorChanged()
{
    if (_editAction)
        reloadActionTitles();
}

void ReadMode::reloadActionTitles()
{
    _editAction->setTitle(tr("Edit"));
    _openAction->setTitle(tr("Open"));
//    _findAction->setTitle(tr("Find"));
    _cloneAction->setTitle(tr("Clone"));
    _closeAction->setTitle(tr("Close"));
}
