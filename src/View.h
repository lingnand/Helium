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
        class TitleBar;
        class Header;
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
class Project;

class View : public bb::cascades::Tab
{
    Q_OBJECT
public:
    static int highlightRangeLimit;

    View(Project *project, Buffer *buffer=NULL);
    virtual ~View();
    bb::cascades::Page *page() const { return _page; }
    bb::cascades::TextArea *textArea() const { return _textArea; }
    Buffer *buffer() const { return _buffer; }
    bb::cascades::NavigationPane *content() const { return _content; }
    MultiViewPane *parent() const {
        return (MultiViewPane *) QObject::parent();
    }
    bb::cascades::NavigationPane *detachContent();
    void reattachContent();
    void hideAllPageActions();
    bool untouched() const;
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
    Q_SLOT void closeProject();
    Q_SLOT void onDeactivated();
    Q_SLOT void onActivated();
    Q_SLOT void onTranslatorChanged();
    // wrappers over the linked buffer
    Q_SLOT void setAutodetectFiletype(bool);
    Q_SLOT void setFiletype(Filetype *);
    Q_SLOT void setName(const QString &);
    Q_SLOT void autoFocus();
    Q_SLOT void resetHeaderTitle(int viewIndex, int viewsSize);
    Q_SLOT void resetHeaderSubtitle(int projectIndex, int projectsSize);
    Q_SLOT void blockPageKeyListener(bool);
    // wrappers over the page
    void setPageTitleBar(bb::cascades::TitleBar *);
    Q_SLOT void undo();
    Q_SLOT void redo();
    Q_SLOT void reload();
Q_SIGNALS:
    void outOfView();
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
    FindMode *_findMode;
    /** project **/
    Project *_project;
    Buffer *_buffer;
    Range _highlightRange;
    bb::cascades::pickers::FilePicker *_fpicker;
    bb::cascades::Header *_header;
    bb::cascades::TextArea *_textArea;
    bb::cascades::ProgressIndicator *_progressIndicator;
    bb::cascades::KeyListener *_pageKeyListener;
    bb::cascades::Page *_page;
    bb::cascades::NavigationPane *_content;
    /** buffer and highlight **/
    ParserPosition _highlightStart;
    QTimer _partialHighlightUpdateTimer;

    int _highlightRangeLimit;
    NormalMode *_normalMode;

    void setMode(ViewMode *);
    void scrollTo(int cursorPosition);
    void scrollByLine(int offset);
    Q_SLOT void resetHeaderTitle();
    Range partialHighlightRange(const BufferState &st, Range focus);
    bb::cascades::pickers::FilePicker *filePicker();
    Q_SLOT void reloadTitle();
    /** callbacks **/
    Q_SLOT void onShouldHideActionBarChanged(bool);
    Q_SLOT void onFileSelected(const QStringList &files);
    Q_SLOT void onPageKeyPressed(bb::cascades::KeyEvent *event);
    Q_SLOT void onTextAreaTextChanged(const QString &text);
    Q_SLOT void onTextAreaCursorPositionChanged();
    Q_SLOT void onBufferFiletypeChanged(Filetype *change, Filetype *old=NULL);
    Q_SLOT void onBufferStateChanged(const StateChangeContext &, const BufferState &);
    Q_SLOT void onBufferProgressChanged(float, bb::cascades::ProgressIndicatorState::Type, const QString &msg);
    Q_SLOT void onProgressMessageDismissed(bb::system::SystemUiResult::Type);
    Q_SLOT void onUnsavedChangeDialogFinishedWhenClosing(bb::system::SystemUiResult::Type);
    Q_SLOT void onTopChanged(bb::cascades::Page *);
};

#endif /* VIEW_H_ */
