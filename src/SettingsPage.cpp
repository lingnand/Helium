/*
 * SettingsPage.cpp
 *
 *  Created on: May 24, 2015
 *      Author: lingnan
 */

#include <bb/cascades/ListView>
#include <bb/cascades/TitleBar>
#include <SettingsPage.h>
#include <GeneralSettingsPage.h>
#include <AppearanceSettingsPage.h>
#include <FiletypeMapSettingsPage.h>
#include <Utility.h>

using namespace bb::cascades;

SettingsPage::SettingsPage(GeneralSettings *generalSettings, AppearanceSettings *appearanceSettings, FiletypeMap *filetypeMap, QObject *parent):
    RepushablePage(parent),
    _generalSettings(generalSettings),
    _generalSettingsPage(NULL),
    _appearanceSettings(appearanceSettings),
    _appearanceSettingsPage(NULL),
    _filetypeMap(filetypeMap),
    _filetypeMapSettingsPage(NULL)
{
    setTitleBar(TitleBar::create());
    ListView *listView = ListView::create().dataModel(&_model);
    conn(listView, SIGNAL(triggered(QVariantList)),
        this, SLOT(onTriggered(QVariantList)));
    setContent(listView);
    onTranslatorChanged();
}

void SettingsPage::onTriggered(QVariantList indexPath)
{
    switch (indexPath[0].toInt()) {
        case 0:
            if (!_generalSettingsPage) {
                _generalSettingsPage = new GeneralSettingsPage(_generalSettings, this);
                conn(_generalSettingsPage, SIGNAL(toPop()),
                    this, SIGNAL(toPop()));
            }
            _generalSettingsPage->setParent(NULL);
            emit toPush(_generalSettingsPage);
            break;
        case 1:
            if (!_appearanceSettingsPage) {
                _appearanceSettingsPage = new AppearanceSettingsPage(_appearanceSettings, this);
                conn(_appearanceSettingsPage, SIGNAL(toPop()),
                    this, SIGNAL(toPop()));
            }
            _appearanceSettingsPage->setParent(NULL);
            emit toPush(_appearanceSettingsPage);
            break;
        case 2:
            if (!_filetypeMapSettingsPage) {
                _filetypeMapSettingsPage = new FiletypeMapSettingsPage(_filetypeMap, this);
                conn(_filetypeMapSettingsPage, SIGNAL(toPush(bb::cascades::Page*)),
                    this, SIGNAL(toPush(bb::cascades::Page*)));
                conn(_filetypeMapSettingsPage, SIGNAL(toPop()),
                    this, SIGNAL(toPop()));
            }
            _filetypeMapSettingsPage->setParent(NULL);
            emit toPush(_filetypeMapSettingsPage);
            break;
    }
}

void SettingsPage::onTranslatorChanged()
{
    titleBar()->setTitle(tr("Settings"));
    _model.clear();
    _model.append(tr("General"));
    _model.append(tr("Appearance"));
    _model.append(tr("Filetypes"));
    if (_generalSettingsPage) {
        _generalSettingsPage->onTranslatorChanged();
    }
    if (_appearanceSettingsPage) {
        _appearanceSettingsPage->onTranslatorChanged();
    }
    if (_filetypeMapSettingsPage) {
        _filetypeMapSettingsPage->onTranslatorChanged();
    }
}
