/*
 * FiletypeMapSettingsPage.cpp
 *
 *  Created on: May 26, 2015
 *      Author: lingnan
 */

#include <bb/cascades/ListView>
#include <bb/cascades/TitleBar>
#include <bb/cascades/ScrollView>
#include <bb/cascades/Page>
#include <FiletypeMapSettingsPage.h>
#include <FiletypeSettingsUI.h>
#include <FiletypeMap.h>
#include <Filetype.h>
#include <Utility.h>

using namespace bb::cascades;

FiletypeMapSettingsPage::FiletypeMapSettingsPage(FiletypeMap *map, QObject *parent):
    RepushablePage(parent),
    _filetypes(map->filetypes()),
    _filetypeSettingsUI(new FiletypeSettingsUI),
    _filetypeSettingsPage(new RepushablePage(this))
{
    setTitleBar(TitleBar::create());
    for (int i = 0; i < _filetypes.size(); i++) {
        _model.append(_filetypes[i]->name());
    }
    ListView *listView = ListView::create().dataModel(&_model);
    conn(listView, SIGNAL(triggered(QVariantList)),
        this, SLOT(onTriggered(QVariantList)));
    setContent(listView);

    _filetypeSettingsPage->setTitleBar(TitleBar::create());
    _filetypeSettingsPage->setContent(ScrollView::create(_filetypeSettingsUI)
        .scrollMode(ScrollMode::Vertical));
    conn(_filetypeSettingsPage, SIGNAL(toPop()),
        this, SIGNAL(toPop()));

    onTranslatorChanged();
}

void FiletypeMapSettingsPage::onTriggered(QVariantList indexPath)
{
    Filetype *ft = _filetypes[indexPath[0].toInt()];
    _filetypeSettingsUI->setFiletype(ft);
    QString title(ft->name());
    title[0] = title[0].toUpper();
    _filetypeSettingsPage->titleBar()->setTitle(title);
    _filetypeSettingsPage->setParent(NULL);
    emit toPush(_filetypeSettingsPage);
}

void FiletypeMapSettingsPage::onTranslatorChanged()
{
    titleBar()->setTitle(tr("Filetypes"));
}
