/*
 * SettingsPage.cpp
 *
 *  Created on: May 24, 2015
 *      Author: lingnan
 */

#include <bb/cascades/ListView>
#include <bb/cascades/TitleBar>
#include <bb/cascades/Page>
#include <bb/cascades/Sheet>
#include <bb/cascades/ActionItem>
#include <bb/cascades/Shortcut>
#include <SettingsPage.h>
#include <GeneralSettingsPage.h>
#include <AppearanceSettingsPage.h>
#include <FiletypeMapSettingsPage.h>
#include <GitSettingsPage.h>
#include <Utility.h>

using namespace bb::cascades;

SettingsPage::SettingsPage(GeneralSettings *generalSettings, AppearanceSettings *appearanceSettings,
        FiletypeMap *filetypeMap, GitSettings *gitSettings):
    _generalSettings(generalSettings),
    _generalSettingsPage(NULL),
    _appearanceSettings(appearanceSettings),
    _appearanceSettingsPage(NULL),
    _filetypeMap(filetypeMap),
    _filetypeMapSettingsPage(NULL),
    _gitSettings(gitSettings),
    _gitSettingsPage(NULL),
    _base(Page::create()
        .titleBar(TitleBar::create()
            .dismissAction(ActionItem::create()
                .onTriggered(this, SLOT(closeSheet()))))
        .addShortcut(Shortcut::create().key("x")
            .onTriggered(this, SLOT(closeSheet()))))
{
    ListView *listView = ListView::create().dataModel(&_model);
    conn(listView, SIGNAL(triggered(QVariantList)),
        this, SLOT(onTriggered(QVariantList)));
    _base->setContent(listView);
    push(_base);
    onTranslatorChanged();
}

void SettingsPage::closeSheet()
{
    ((Sheet *) parent())->close();
}

void SettingsPage::onTriggered(QVariantList indexPath)
{
    switch (indexPath[0].toInt()) {
        case 0:
            if (!_generalSettingsPage) {
                _generalSettingsPage = new GeneralSettingsPage(_generalSettings);
            }
            push(_generalSettingsPage);
            break;
        case 1:
            if (!_appearanceSettingsPage) {
                _appearanceSettingsPage = new AppearanceSettingsPage(_appearanceSettings);
            }
            push(_appearanceSettingsPage);
            break;
        case 2:
            if (!_filetypeMapSettingsPage) {
                _filetypeMapSettingsPage = new FiletypeMapSettingsPage(_filetypeMap);
            }
            push(_filetypeMapSettingsPage);
            break;
        case 3:
            if (!_gitSettingsPage) {
                _gitSettingsPage = new GitSettingsPage(_gitSettings);
            }
            push(_gitSettingsPage);
            break;
    }
}

void SettingsPage::onTranslatorChanged()
{
    _base->titleBar()->setTitle(tr("Settings"));
    _base->titleBar()->dismissAction()->setTitle(tr("Close"));
    _model.clear();
    _model << tr("General")
           << tr("Appearance")
           << tr("Filetypes")
           << tr("Git");
    if (_generalSettingsPage)
        _generalSettingsPage->onTranslatorChanged();
    if (_appearanceSettingsPage)
        _appearanceSettingsPage->onTranslatorChanged();
    if (_filetypeMapSettingsPage)
        _filetypeMapSettingsPage->onTranslatorChanged();
    if (_gitSettingsPage)
        _gitSettingsPage->onTranslatorChanged();
}
