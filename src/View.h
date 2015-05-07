/*
 * View.h
 *
 *  Created on: Nov 21, 2014
 *      Author: lingnan
 */

#ifndef VIEW_H_
#define VIEW_H_

#include <QTimer>
#include <bb/cascades/Tab>
#include <bb/system/SystemUiResult>
#include <bb/cascades/ProgressIndicatorState>
#include <StateChangeContext.h>
#include <BufferState.h>

namespace bb {
    namespace cascades {
        class TextEditor;
        class TextArea;
        class Page;
        class ProgressIndicator;
        class KeyEvent;
        namespace pickers {
            class FilePicker;
        }
    }
}

class MultiViewPane;
class ModKeyListener;
class Buffer;
class ViewMode;
class NormalMode;
class FindMode;

class View : public bb::cascades::Tab
{
    Q_OBJECT
public:
    static int highlightRangeLimit;

    View(Buffer *buffer=NULL);
    virtual ~View() {}
    bb::cascades::Page *page() const;
    void detachPage();
    void reattachPage();
    void hideAllPageActions();
    MultiViewPane *parent() const;
    Buffer *buffer() const;
    bb::cascades::TextArea *textArea() const;
    ModKeyListener *textAreaModKeyListener() const;
    Q_SLOT void setBuffer(Buffer* buffer);
    Q_SLOT void setHighlightRangeLimit(int limit);
    Q_SLOT void setNormalMode();
    Q_SLOT void setFindMode();
    enum SaveStatus {OpenedFilePicker, Finished};
    Q_SLOT SaveStatus save();
    Q_SLOT SaveStatus saveAs();
    Q_SLOT void open();
    Q_SLOT void handleTextControlBasicModifiedKeys(bb::cascades::TextEditor *editor, bb::cascades::KeyEvent *event);
    Q_SLOT void updateTextAreaPartialHighlight();
    Q_SLOT void onOutOfView();
    Q_SLOT void onTranslatorChanged();
Q_SIGNALS:
    void undo();
    void redo();
    void hasUndosChanged(bool);
    void hasRedosChanged(bool);
    void translatorChanged();
    void bufferNameChanged(const QString &);
    void bufferFilepathChanged(const QString &);
    void bufferDirtyChanged(bool);
    void bufferLockedChanged(bool);
private:
    /** mode **/
    ViewMode *_mode;
    NormalMode *_normalMode;
    FindMode *_findMode;
    void setMode(ViewMode *);

    bb::cascades::Page *_page;
    bb::cascades::TextArea *_textArea;
    ModKeyListener *_textAreaModKeyListener;
    bb::cascades::ProgressIndicator *_progressIndicator;

    /** file related **/
    bb::cascades::pickers::FilePicker *_fpicker;
    bb::cascades::pickers::FilePicker *filePicker();
    void pickFileToOpen();
    Q_SLOT void onFileSelected(const QStringList &files);

    /** navigation **/
    void scrollTo(int cursorPosition);
    void scrollByLine(int offset);
    Q_SLOT void oneLineUp();
    Q_SLOT void oneLineDown();
    Q_SLOT void scrollToTop();
    Q_SLOT void scrollToBottom();
    Q_SLOT void scrollUp();
    Q_SLOT void scrollDown();

    /** view management **/
    Q_SLOT void clone();
    Q_SLOT void close();

    /** buffer and highlight **/
    Buffer *_buffer;
    int _highlightRangeLimit;
    Range _highlightRange;
    ParserPosition _highlightStart;
    QTimer _partialHighlightUpdateTimer;
    Range partialHighlightRange(const BufferState &st, Range focus);

    /** text operation **/
    Q_SLOT void killCurrentLine();

    /** callbacks **/
    Q_SLOT void reloadTitle();
    Q_SLOT void onTextAreaTextChanged(const QString &text);
    Q_SLOT void onTextAreaModKeyPressed(bb::cascades::KeyEvent *event);
    Q_SLOT void onTextAreaModifiedKeyPressed(bb::cascades::KeyEvent *event, ModKeyListener *listener);
    Q_SLOT void onTextAreaCursorPositionChanged();
    Q_SLOT void onBufferFiletypeChanged(const QString &filetype, bool toast=true);
    Q_SLOT void onBufferStateChanged(const StateChangeContext &, const BufferState &);
    Q_SLOT void onBufferProgressChanged(float, bb::cascades::ProgressIndicatorState::Type, const QString &msg);
    Q_SLOT void onProgressMessageDismissed(bb::system::SystemUiResult::Type);
    Q_SLOT void onBufferSavedToFile(const QString &filename);
    Q_SLOT void onUnsavedChangeDialogFinishedWhenOpening(bb::system::SystemUiResult::Type);
    Q_SLOT void onUnsavedChangeDialogFinishedWhenClosing(bb::system::SystemUiResult::Type);
    Q_SLOT void autoFocus();
};

#endif /* VIEW_H_ */
