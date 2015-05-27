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
#include <FiletypeMapSettingsPage.h>
#include <Utility.h>

using namespace bb::cascades;

SettingsPage::SettingsPage(GeneralSettings *generalSettings, FiletypeMap *filetypeMap, QObject *parent):
    RepushablePage(parent),
    _title(TitleBar::create()),
    _generalSettings(generalSettings),
    _generalSettingsPage(NULL),
    _filetypeMap(filetypeMap),
    _filetypeMapSettingsPage(NULL)
{
    setTitleBar(_title);
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
            qDebug() << "Appearance setting page should be pushed";
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
    _title->setTitle(tr("Settings"));
    _model.clear();
    _model.append(tr("General"));
    _model.append(tr("Appearance"));
    _model.append(tr("Filetypes"));
}
