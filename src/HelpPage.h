/*
 * HelpPage.h
 *
 *  Created on: Jun 7, 2015
 *      Author: lingnan
 */

#ifndef HELPPAGE_H_
#define HELPPAGE_H_

#include <bb/cascades/NavigationPane>
#include <PushablePage.h>

namespace bb {
    namespace cascades {
        class Label;
        class Header;
        class Option;
        class ScrollView;
        class ActionItem;
        class NavigationPane;
    }
}

class Segment;

class HelpPage : public bb::cascades::NavigationPane
{
    Q_OBJECT
public:
    enum Mode { Reference, ChangeList };
    HelpPage();
    void setMode(Mode);
    Q_SLOT void onTranslatorChanged();
private:
    bb::cascades::Option *_referenceOption;
    bb::cascades::Option *_changeListOption;
    bb::cascades::Header *_tabsAndBuffers;
    bb::cascades::Label *_tabsAndBuffersDesc;
    bb::cascades::Header *_projectsManagement;
    bb::cascades::Label *_projectsManagementDesc;
    bb::cascades::Header *_fileManagement;
    bb::cascades::Label *_fileManagementDesc;
    bb::cascades::Header *_keyboardShortcuts;
    bb::cascades::Label *_keyboardShortcutsDesc;
    bb::cascades::Header *_highlight;
    bb::cascades::Label *_highlightDesc;
    bb::cascades::Header *_findAndReplace;
    bb::cascades::Label *_findAndReplaceDesc;
    bb::cascades::Header *_filetype;
    bb::cascades::Label *_filetypeDesc;
    bb::cascades::Header *_runProfile;
    bb::cascades::Label *_runProfileDesc;
    Segment *_referenceSegment;
    bb::cascades::Label *_changeList;
    Segment *_changeListSegment;
    bb::cascades::ScrollView *_view;
    bb::cascades::ActionItem *_rateAction;
    bb::cascades::Page *_base;
    struct ContentPage : public PushablePage {
        bb::cascades::Label *contentLabel;
        ContentPage();
    } *_contentPage;

    Q_SLOT void closeSheet();
    Q_SLOT void onSelectedOptionChanged(bb::cascades::Option *);
    Q_SLOT void onTabsAndBuffersHeaderClicked();
    void loadPage(const QString &title, const QString &content);
};

#endif /* HELPPAGE_H_ */
