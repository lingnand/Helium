/*
 * View.h
 *
 *  Created on: Nov 21, 2014
 *      Author: lingnan
 */

#ifndef VIEW_H_
#define VIEW_H_

#include <QObject>
#include <boost/regex.hpp>
#include <bb/cascades/Tab>

namespace bb {
    namespace cascades {
        class TextField;
        class TextArea;
        class Page;
        class ProgressIndicator;
        class KeyEvent;
        class ActionItem;
        class TitleBar;
        class TitleBarExpandableArea;
        class Button;
        class CheckBox;
    }
}

class Buffer;

class View : public bb::cascades::Tab
{
    Q_OBJECT
public:
    View(Buffer* buffer);
    virtual ~View() {}
    Q_SLOT void onLanguageChanged();
    Q_SLOT void setBuffer(Buffer* buffer);
    Q_SLOT void onOutOfView();
private:
    enum ViewMode { Normal, Find };
    ViewMode _mode;
    bb::cascades::Page *_page;

    bb::cascades::TitleBar *_titleBar;
    bb::cascades::TitleBar *_findTitleBar;

    bb::cascades::TextField *_titleField;
    bb::cascades::TextArea *_textArea;
    bb::cascades::ProgressIndicator *_progressIndicator;

    // find and replace
    bb::cascades::TitleBarExpandableArea *_findExpandableArea;
    bb::cascades::TextField *_findField;
    bb::cascades::TextField *_replaceField;
    bb::cascades::CheckBox *_findCaseSensitiveCheckBox;

    bb::cascades::ActionItem *_saveAction;
    bb::cascades::ActionItem *_undoAction;
    bb::cascades::ActionItem *_redoAction;
    bb::cascades::ActionItem *_findAction;

    bb::cascades::ActionItem *_findPrevAction;
    bb::cascades::ActionItem *_findNextAction;
    bb::cascades::ActionItem *_replaceNextAction;
    bb::cascades::ActionItem *_replaceAllAction;
    bb::cascades::ActionItem *_findCancelAction;

    // the find state to keep track of the current find
    typedef QPair<int, int> TextSelection;
    QString _findQuery;
    std::string _findBuffer;
    bool _lastFindLoop;
    boost::sregex_iterator _findIterator;
    boost::regex _findRegex;
    bool _findComplete;
    QList<TextSelection> _findHits;
    int _findOffset;
    int _findIndex;
    int _bofIndex;
    QString _replaceQuery;

    bool _modUsed;
    Buffer *_buffer;

    Q_SLOT void select(const TextSelection &selection);
    Q_SLOT void setNormalModeActions();
    Q_SLOT void setFindModeActions();
    Q_SLOT void reloadNormalModeActionTitles();
    Q_SLOT void reloadFindModeActionTitles();
    Q_SLOT bool findModeOn();
    Q_SLOT bool findModeOff();
    Q_SLOT void findNext();
    Q_SLOT void findPrev();
    Q_SLOT void replaceNext();
    Q_SLOT void replaceAll();
    Q_SLOT void onModPressTimeout();
    Q_SLOT void onTitleFieldFocusChanged(bool focus);
    Q_SLOT void onTextAreaTextChanged(const QString& text);
    Q_SLOT void onTextAreaModKeyPressed(bb::cascades::KeyEvent *event);
    Q_SLOT void onNormalModeModifiedKeyPressed(bb::cascades::KeyEvent *event);
    Q_SLOT void onFindOptionButtonClicked();
    Q_SLOT void onBufferFiletypeChanged(const QString& filetype);
    Q_SLOT void onBufferContentChanged(const QString& content, int cursorPosition);
    Q_SLOT void onBufferProgressChanged(float progress);
    Q_SLOT void onBufferHasUndosChanged(bool hasUndos);
    Q_SLOT void onBufferHasRedosChanged(bool hasRedos);
    Q_SLOT void onSaveTriggered();
    Q_SLOT void onUndoTriggered();
    Q_SLOT void onRedoTriggered();
    Q_SLOT void autoFocus();
    Q_SLOT void setTitle(const QString& title);
};

#endif /* VIEW_H_ */
