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
        class TextArea;
        class Page;
        class NavigationPane;
        class ProgressIndicator;
        class KeyEvent;
        class KeyListener;
        namespace pickers {
            class FilePicker;
        }
    }
}

class MultiViewPane;
class Buffer;
class ViewMode;
class NormalMode;
class FindMode;
class Filetype;

class View : public bb::cascades::Tab
{
    Q_OBJECT
public:
    static int highlightRangeLimit;

    View(Buffer *buffer=NULL);
    virtual ~View() {}
    bb::cascades::Page *page() const { return _page; }
    bb::cascades::TextArea *textArea() const { return _textArea; }
    Buffer *buffer() const { return _buffer; }
    bb::cascades::NavigationPane *content() const {
        return (bb::cascades::NavigationPane *) Tab::content();
    }
    MultiViewPane *parent() const {
        return (MultiViewPane *) QObject::parent();
    }
    void detachPage();
    void reattachPage();
    void hideAllPageActions();
    bool active() const;
    Q_SLOT void setBuffer(Buffer* buffer);
    Q_SLOT void setHighlightRangeLimit(int limit);
    Q_SLOT void setNormalMode();
    Q_SLOT void setFindMode();
    enum SaveStatus {OpenedFilePicker, Finished};
    Q_SLOT SaveStatus save();
    Q_SLOT SaveStatus saveAs();
    Q_SLOT void open();
    Q_SLOT void updateTextAreaPartialHighlight();
    Q_SLOT void killCurrentLine();
    Q_SLOT void clone();
    Q_SLOT void close();
    Q_SLOT void onOutOfView();
    Q_SLOT void onTranslatorChanged();
    // wrappers over the linked buffer
    Q_SLOT void setAutodetectFiletype(bool);
    Q_SLOT void setFiletype(Filetype *);
Q_SIGNALS:
    void undo();
    void redo();
    void hasUndosChanged(bool);
    void hasRedosChanged(bool);
    void translatorChanged();
    void bufferNameChanged(const QString &);
    void bufferFiletypeChanged(Filetype *change, Filetype *old);
    void bufferFilepathChanged(const QString &);
    void bufferDirtyChanged(bool);
    void bufferLockedChanged(bool);
    void bufferAutodetectFiletypeChanged(bool);
private:
    /** mode **/
    ViewMode *_mode;
    NormalMode *_normalMode;
    FindMode *_findMode;
    void setMode(ViewMode *);

    bb::cascades::Page *_page;
    bb::cascades::KeyListener *_pageKeyListener;
    bb::cascades::TextArea *_textArea;
    bb::cascades::ProgressIndicator *_progressIndicator;

    /** file related **/
    bb::cascades::pickers::FilePicker *_fpicker;
    bb::cascades::pickers::FilePicker *filePicker();
    void pickFileToOpen();
    Q_SLOT void onFileSelected(const QStringList &files);

    /** navigation **/
    void scrollTo(int cursorPosition);
    void scrollByLine(int offset);

    /** buffer and highlight **/
    Buffer *_buffer;
    int _highlightRangeLimit;
    Range _highlightRange;
    ParserPosition _highlightStart;
    QTimer _partialHighlightUpdateTimer;
    Range partialHighlightRange(const BufferState &st, Range focus);

    /** callbacks **/
    Q_SLOT void reloadTitle();
    Q_SLOT void blockPageKeyListener(bool);
    Q_SLOT void onPageKeyPressed(bb::cascades::KeyEvent *event);
    Q_SLOT void onTextAreaTextChanged(const QString &text);
    Q_SLOT void onTextAreaCursorPositionChanged();
    Q_SLOT void onBufferFiletypeChanged(Filetype *change, Filetype *old=NULL);
    Q_SLOT void onBufferStateChanged(const StateChangeContext &, const BufferState &);
    Q_SLOT void onBufferProgressChanged(float, bb::cascades::ProgressIndicatorState::Type, const QString &msg);
    Q_SLOT void onProgressMessageDismissed(bb::system::SystemUiResult::Type);
    Q_SLOT void onBufferSavedToFile(const QString &filename);
    Q_SLOT void onUnsavedChangeDialogFinishedWhenOpening(bb::system::SystemUiResult::Type);
    Q_SLOT void onUnsavedChangeDialogFinishedWhenClosing(bb::system::SystemUiResult::Type);
    Q_SLOT void autoFocus();
};

#endif /* VIEW_H_ */
