/*
 * View.h
 *
 *  Created on: Nov 21, 2014
 *      Author: lingnan
 */

#ifndef VIEW_H_
#define VIEW_H_

#include <QObject>
#include <QTimer>
#include <boost/regex.hpp>
#include <bb/cascades/Tab>
#include <bb/system/SystemUiResult>
#include <bb/cascades/ProgressIndicatorState>
#include <Replacement.h>
#include <StateChangeContext.h>
#include <BufferState.h>

namespace bb {
    namespace cascades {
        class TextField;
        class TextEditor;
        class TextArea;
        class Page;
        class ProgressIndicator;
        class KeyEvent;
        class ActionItem;
        class TitleBar;
        class TitleBarExpandableArea;
        class Button;
        class CheckBox;
        namespace pickers {
            class FilePicker;
        }
    }
}

class MultiViewPane;
class ModKeyListener;
class Buffer;

class View : public bb::cascades::Tab
{
    Q_OBJECT
public:
    View(Buffer *buffer=NULL);
    virtual ~View() {}
    bb::cascades::Page *content() const;
    MultiViewPane *parent() const;
    Q_SLOT void onTranslatorChanged();
    Buffer *buffer() const;
    Q_SLOT void setBuffer(Buffer* buffer);
    Q_SLOT void onOutOfView();
Q_SIGNALS:
    void hasUndosChanged(bool);
    void hasRedosChanged(bool);
private:
    enum ViewMode { Normal, Find } _mode;
    // #### main UI elements
    bb::cascades::Page *_page;

    bb::cascades::TitleBar *_titleBar;
    bb::cascades::TitleBar *_findTitleBar;

    bb::cascades::TextField *_titleField;
    bb::cascades::TextArea *_textArea;
    ModKeyListener *_textAreaModKeyListener;
    bb::cascades::ProgressIndicator *_progressIndicator;

    // #### find and replace (lazy loaded on demand)
    bb::cascades::TitleBarExpandableArea *_findExpandableArea;
    bb::cascades::TextField *_findField;
    bb::cascades::TextField *_replaceField;
    bb::cascades::CheckBox *_findCaseSensitiveCheckBox;

    bb::cascades::ActionItem *_saveAction;
    bb::cascades::ActionItem *_saveAsAction;
    bb::cascades::ActionItem *_openAction;
    bb::cascades::ActionItem *_undoAction;
    bb::cascades::ActionItem *_redoAction;
    bb::cascades::ActionItem *_findAction;
    bb::cascades::ActionItem *_cloneAction;
    bb::cascades::ActionItem *_closeAction;

    bb::cascades::ActionItem *_goToFindFieldAction;
    bb::cascades::ActionItem *_findPrevAction;
    bb::cascades::ActionItem *_findNextAction;
    bb::cascades::ActionItem *_replaceNextAction;
    bb::cascades::ActionItem *_replaceAllAction;
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

    // #### file related
    bb::cascades::pickers::FilePicker *_fpicker;
    bb::cascades::pickers::FilePicker *filePicker();
    enum SaveStatus {OpenedFilePicker, Finished};
    Q_SLOT SaveStatus save();
    Q_SLOT SaveStatus saveAs();
    Q_SLOT void open();
    void pickFileToOpen();
    Q_SLOT void onFileSelected(const QStringList &files);

    // #### view management
    Q_SLOT void clone();
    Q_SLOT void close();

    // #### buffer and highlight
    Buffer *_buffer;
    Range _highlightRange;
    ParserPosition _highlightStart;
    QTimer _partialHighlightUpdateTimer;
    Range partialHighlightRange(const BufferState &st, Range focus);
    Q_SLOT void updateTextAreaPartialHighlight();

    Q_SLOT void select(const TextSelection &selection);
    Q_SLOT void setHistoryActions();
    Q_SLOT void setNormalModeActions();
    Q_SLOT void setFindModeActions();
    Q_SLOT void reloadNormalModeActionTitles();
    Q_SLOT void reloadFindModeActionTitles();
    Q_SLOT void reloadFindTitleBarLabels();
    Q_SLOT bool findModeOn();
    Q_SLOT bool findModeOff();
    Q_SLOT void findNext();
    Q_SLOT void findNextWithOptions(bool interactive, FindQueryUpdateStatus status);
    Q_SLOT void findPrev();
    Q_SLOT FindQueryUpdateStatus updateFindQuery(bool interactive);
    Q_SLOT void replaceNext();
    Q_SLOT void replaceAll();
    // text operations
    Q_SLOT void killCurrentLine();
    Q_SLOT void onReplaceFromTopDialogFinished(bb::system::SystemUiResult::Type);
    Q_SLOT void onFindFieldModifiedKeyPressed(bb::cascades::KeyEvent *event);
    Q_SLOT void onReplaceFieldModifiedKeyPressed(bb::cascades::KeyEvent *event);
    Q_SLOT void onFindFieldsModifiedKeyPressed(bb::cascades::TextEditor *editor, bb::cascades::KeyEvent *event);
    Q_SLOT void onTitleFieldFocusChanged(bool focus);
    Q_SLOT void onTitleFieldModifiedKeyPressed(bb::cascades::KeyEvent *event, ModKeyListener *listener);
    Q_SLOT void onTextAreaTextChanged(const QString& text);
    Q_SLOT void onTextAreaModKeyPressed(bb::cascades::KeyEvent *event);
    Q_SLOT void onTextAreaModifiedKeyPressed(bb::cascades::KeyEvent *event, ModKeyListener *listener);
    Q_SLOT void onTextAreaCursorPositionChanged();
    Q_SLOT void onTextControlModifiedKeyPressed(bb::cascades::TextEditor *editor, bb::cascades::KeyEvent *event);
    Q_SLOT void onFindOptionButtonClicked();
    Q_SLOT void onBufferLockedChanged(bool locked);
    Q_SLOT void onBufferFiletypeChanged(const QString& filetype);
    Q_SLOT void onBufferFilepathChanged(const QString& filepath);
    Q_SLOT void onBufferStateChanged(const StateChangeContext &, const BufferState &);
    Q_SLOT void onBufferProgressChanged(float, bb::cascades::ProgressIndicatorState::Type, const QString &msg);
    Q_SLOT void onProgressMessageDismissed(bb::system::SystemUiResult::Type);
    Q_SLOT void onBufferSavedToFile(const QString &filename);
    Q_SLOT void onBufferDirtyChanged(bool dirty);
    Q_SLOT void onUnsavedChangeDialogFinishedWhenOpening(bb::system::SystemUiResult::Type);
    Q_SLOT void onUnsavedChangeDialogFinishedWhenClosing(bb::system::SystemUiResult::Type);
    Q_SLOT void onUndoTriggered();
    Q_SLOT void onRedoTriggered();
    Q_SLOT void autoFocus();
    Q_SLOT void setTitle(const QString& title);
};

#endif /* VIEW_H_ */
