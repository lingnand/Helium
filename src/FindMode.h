/*
 * FindMode.h
 *
 *  Created on: May 4, 2015
 *      Author: lingnan
 */

#ifndef FINDMODE_H_
#define FINDMODE_H_

#include <bb/system/SystemUiResult>
#include <boost/regex.hpp>
#include <ViewMode.h>
#include <Replacement.h>

namespace bb {
    namespace cascades {
        class TextField;
        class TitleBar;
        class TitleBarExpandableArea;
        class ActionItem;
        class KeyEvent;
        class Button;
        class CheckBox;
        class TextEditor;
    }
}

class FindMode: public ViewMode
{
    Q_OBJECT
public:
    FindMode(View *);
    void autoFocus(bool goToModeControl);
    Q_SLOT void setLocked(bool);
    void onEnter();
    void onExit();
private:
    bb::cascades::TitleBar *_findTitleBar;
    bb::cascades::TitleBarExpandableArea *_findExpandableArea;
    bb::cascades::TextField *_findField;
    bb::cascades::TextField *_replaceField;
    bb::cascades::CheckBox *_findCaseSensitiveCheckBox;
    bb::cascades::Button *_findCancelButton;

    bb::cascades::ActionItem *_goToFindFieldAction;
    bb::cascades::ActionItem *_findPrevAction;
    bb::cascades::ActionItem *_findNextAction;
    bb::cascades::ActionItem *_replaceNextAction;
    bb::cascades::ActionItem *_replaceAllAction;
    bb::cascades::ActionItem *_undoAction;
    bb::cascades::ActionItem *_redoAction;
    bb::cascades::ActionItem *_findCancelAction;

    // the find state to keep track of the current find
    enum FindQueryUpdateStatus { Changed, Unchanged, Invalid };
    QString _findQuery;
    bool _findBufferDirty;
    std::wstring _findBuffer;
    boost::wsregex_iterator _findIterator;
    boost::wregex _findRegex;
    bool _findComplete;
    struct FindMatch {
        TextSelection selection;
        boost::wsmatch match;
        FindMatch(TextSelection _sel, boost::wsmatch _match): selection(_sel), match(_match) {}
    };
    QList<FindMatch> _findHits;
    int _findIndex;
    // negative means the top/first match is still indeterminate
    int _bofIndex;
    QString _replaceQuery;
    QList<Replacement> _replaces;
    int _numberOfReplacesTillBottom;

    void select(const TextSelection &selection);
    Q_SLOT void findNext();
    void findNextWithOptions(bool interactive, FindQueryUpdateStatus status);
    FindQueryUpdateStatus updateFindQuery(bool interactive);
    Q_SLOT void findPrev();
    Q_SLOT void replaceNext();
    Q_SLOT void replaceAll();

    Q_SLOT void onReplaceFromTopDialogFinished(bb::system::SystemUiResult::Type);
    Q_SLOT void onFindFieldModifiedKeyPressed(bb::cascades::KeyEvent *event);
    Q_SLOT void onReplaceFieldModifiedKeyPressed(bb::cascades::KeyEvent *event);
    Q_SLOT void onFindFieldsModifiedKeyPressed(bb::cascades::TextEditor *editor, bb::cascades::KeyEvent *event);
    Q_SLOT void onFindOptionButtonClicked();

    Q_SLOT void onTranslatorChanged();
    void reloadActionTitles();
};

#endif /* FINDMODE_H_ */
