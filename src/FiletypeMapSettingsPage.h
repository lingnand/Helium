/*
 * FiletypeMapSettingsPage.h
 *
 *  Created on: May 26, 2015
 *      Author: lingnan
 */

#ifndef FILETYPEMAPSETTINGSPAGE_H_
#define FILETYPEMAPSETTINGSPAGE_H_

#include <bb/cascades/Page>
#include <bb/cascades/QListDataModel>
#include <PushablePage.h>

class Filetype;
class FiletypeMap;
class FiletypeSettingsUI;

class FiletypeMapSettingsPage : public PushablePage
{
    Q_OBJECT
public:
    FiletypeMapSettingsPage(FiletypeMap *);
    Q_SLOT void onTranslatorChanged();
private:
    QList<Filetype *> _filetypes;
    FiletypeSettingsUI *_filetypeSettingsUI;

    bb::cascades::QListDataModel<QString> _model;
    PushablePage *_filetypeSettingsPage;
    Q_SLOT void onTriggered(QVariantList);
};

#endif /* FILETYPEMAPSETTINGSPAGE_H_ */
