/*
 * Settings.cpp
 *
 *  Created on: May 24, 2015
 *      Author: lingnan
 */

#include <bb/cascades/ListView>
#include <bb/cascades/TitleBar>
#include <Settings.h>
#include <GeneralSettings.h>
#include <Utility.h>

using namespace bb::cascades;

Settings::Settings():
    _title(TitleBar::create()),
    _generalSettings(NULL)
{
    setTitleBar(_title);
    ListView *listView = ListView::create()
        .dataModel(&_model);
    conn(listView, SIGNAL(triggered(QVariantList)),
        this, SLOT(onTriggered(QVariantList)));
    setContent(listView);
    onTranslatorChanged();
}

void Settings::onTriggered(QVariantList indexPath)
{
    switch (indexPath[0].toInt()) {
        case 0:
            if (!_generalSettings) {
                _generalSettings = new GeneralSettings;
            }
            emit pageSelected(_generalSettings);
            break;
        case 1:
            qDebug() << "Appearance setting page should be pushed";
            break;
        case 2:
            qDebug() << "Filetypes setting page should be pushed";
            break;
    }
}

void Settings::onTranslatorChanged()
{
    _title->setTitle(tr("Settings"));
    _model.clear();
    _model.append(tr("General"));
    _model.append(tr("Appearance"));
    _model.append(tr("Filetypes"));
}
