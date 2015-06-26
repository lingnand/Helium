/*
 * HelpPage.h
 *
 *  Created on: Jun 7, 2015
 *      Author: lingnan
 */

#ifndef HELPPAGE_H_
#define HELPPAGE_H_

#include <QSignalMapper>
#include <RepushablePage.h>

namespace bb {
    namespace cascades {
        class Label;
        class Header;
    }
}
class HelpPage : public RepushablePage
{
    Q_OBJECT
public:
    HelpPage(QObject *parent=NULL);
    Q_SLOT void onTranslatorChanged();
private:
    bb::cascades::Header *_tabsAndBuffers;
    bb::cascades::Label *_tabsAndBuffersDesc;
    bb::cascades::Header *_fileManagement;
    bb::cascades::Label *_fileManagementDesc;
    bb::cascades::Header *_highlight;
    bb::cascades::Label *_highlightDesc;
    bb::cascades::Header *_filetype;
    bb::cascades::Label *_filetypeDesc;
    bb::cascades::Header *_findAndReplace;
    bb::cascades::Label *_findAndReplaceDesc;
    struct ContentPage : public RepushablePage {
        bb::cascades::Label *contentLabel;
        ContentPage(QObject *parent=NULL);
    } *_contentPage;

    Q_SLOT void onTabsAndBuffersHeaderClicked();
    Q_SLOT void onFileManagementHeaderClicked();
    Q_SLOT void onHighlightHeaderClicked();
    Q_SLOT void onFiletypeHeaderClicked();
    Q_SLOT void onFindAndReplaceHeaderClicked();
    void loadPage(const QString &title, const QString &content);
};

#endif /* HELPPAGE_H_ */
