/*
 * View.cpp
 *
 *  Created on: Nov 21, 2014
 *      Author: lingnan
 */

#include <bb/cascades/Page>
#include <bb/cascades/Container>
#include <bb/cascades/ProgressIndicator>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/TextArea>
#include <bb/cascades/KeyEvent>
#include <bb/cascades/Shortcut>
#include <bb/cascades/pickers/FilePicker>
#include <bb/system/Clipboard>
#include <View.h>
#include <MultiViewPane.h>
#include <Buffer.h>
#include <ModKeyListener.h>
#include <SignalBlocker.h>
#include <NormalMode.h>
#include <FindMode.h>
#include <Utility.h>

using namespace bb::cascades;

#define SCROLL_RANGE 30

#define HIGHLIGHT_RANGE_LIMIT 20
// totalResource = plainTextSize + c * diffLimit
// we are now assumign c to be 5
// we have (for playlist.c): plainTextSize = 42000, diffLimit = 15000
// so totalResource = 42000 + 5 * 15000 = 117000
#define DIFF_LIMIT(plainTextSize) ((118000 - plainTextSize) / 5)

// keys
#define KEYFLAG_NONE 0
#define KEYFLAG_RETURN 1
// the one side range for partial highlight (will be put into settings)

// #### View
// TODO: it would be good if you can modulize the undo/redo functionality
// and then attach it to each textfield/area like what you did with the modkeylistener!
// that would be awesome...
View::View(Buffer *buffer):
    _mode(NULL), _findMode(NULL),
    _buffer(NULL),
    _fpicker(NULL),
    _highlightRange(0, 1),
    _highlightRangeLimit(HIGHLIGHT_RANGE_LIMIT),
    _textArea(TextArea::create()
        .format(TextFormat::Html)
        .inputFlags(TextInputFlag::SpellCheckOff
                    | TextInputFlag::AutoCorrectionOff
                    | TextInputFlag::AutoCapitalizationOff
                    | TextInputFlag::AutoPeriodOff)
        .contentFlags(TextContentFlag::ActiveTextOff)
        .focusPolicy(FocusPolicy::Touch)
        .layoutProperties(StackLayoutProperties::create()
            .spaceQuota(1))
        .bottomMargin(0)),
    _textAreaModKeyListener(ModKeyListener::create(KEYCODE_RETURN)
        .onModifiedKeyPressed(this, SLOT(onTextAreaModifiedKeyPressed(bb::cascades::KeyEvent*, ModKeyListener*)))
        .onModKeyPressed(this, SLOT(onTextAreaModKeyPressed(bb::cascades::KeyEvent*)))
        .onTextAreaInputModeChanged(_textArea, SLOT(setInputMode(bb::cascades::TextAreaInputMode::Type)))
        .modOffOn(_textArea, SIGNAL(focusedChanged(bool)))),
    _progressIndicator(ProgressIndicator::create()
        .visible(false)
        .vertical(VerticalAlignment::Bottom)
        .topMargin(0))
{
    _textArea->addKeyListener(_textAreaModKeyListener);
    conn(_textArea->editor(), SIGNAL(cursorPositionChanged(int)),
            this, SLOT(onTextAreaCursorPositionChanged()));
    conn(_textArea, SIGNAL(textChanging(const QString)),
        this, SLOT(onTextAreaTextChanged(const QString&)));

    // setup the timer for partial highlight update
    _partialHighlightUpdateTimer.setSingleShot(true);
    conn(&_partialHighlightUpdateTimer, SIGNAL(timeout()),
            this, SLOT(updateTextAreaPartialHighlight()));

    // the page
    setContent(Page::create()
        .content(Container::create()
            .add(_textArea)
            .add(_progressIndicator))
        .actionBarVisibility(ChromeVisibility::Visible)
        .addShortcut(Shortcut::create().key("Enter")
            .onTriggered(this, SLOT(autoFocus())))
        // attach the touch keyboard handler to the page so
        // that we can enable touch-less scrolling
        .addShortcut(Shortcut::create().key("j")
            .onTriggered(this, SLOT(oneLineDown())))
        .addShortcut(Shortcut::create().key("k")
            .onTriggered(this, SLOT(oneLineUp())))
        .addShortcut(Shortcut::create().key("t")
            .onTriggered(this, SLOT(scrollToTop())))
        .addShortcut(Shortcut::create().key("b")
            .onTriggered(this, SLOT(scrollToBottom())))
        .addShortcut(Shortcut::create().key("Space")
            .onTriggered(this, SLOT(scrollDown())))
        .addShortcut(Shortcut::create().key("u") // for passport users
            .onTriggered(this, SLOT(scrollUp())))
        .addShortcut(Shortcut::create().key("Shift+Space")
            .onTriggered(this, SLOT(scrollUp()))));

    setBuffer(buffer);

    onTranslatorChanged();
    setMode(_normalMode = new NormalMode(this));
}

void View::setMode(ViewMode *mode)
{
    if (mode != _mode) {
        if (_mode) {
            _mode->onExit();
        }
        if (mode) {
            mode->onEnter(_mode);
        }
        _mode = mode;
    }
}

TextArea *View::textArea() const
{
    return _textArea;
}

ModKeyListener *View::textAreaModKeyListener() const
{
    return _textAreaModKeyListener;
}

Page *View::content() const
{
    return (Page *) bb::cascades::Tab::content();
}

MultiViewPane *View::parent() const
{
    return (MultiViewPane *) QObject::parent();
}

Buffer *View::buffer() const
{
    return _buffer;
}

void View::onOutOfView()
{
    emit outOfView();
}

void View::autoFocus()
{
    _mode->autoFocus();
}

void View::setNormalMode()
{
    setMode(_normalMode);
}

void View::setFindMode()
{
    if (!_findMode) {
        _findMode = new FindMode(this);
    }
    setMode(_findMode);
}

void View::reloadTitle()
{
    QString namefmt = _buffer->dirty() ? "%1*" : "%1";
    if (_buffer->name().isEmpty()) {
        Tab::setTitle(namefmt.arg(tr("No Name")));
    } else {
        Tab::setTitle(namefmt.arg(_buffer->name()));
    }
}

void View::setBuffer(Buffer *buffer)
{
    if (buffer != _buffer) {
        if (_buffer) {
            _buffer->detachView(this);
            _buffer->disconnect(this);
            disconnect(_buffer);
            // remove the buffer if it no longer has other views attached
            if (_buffer->views().empty())
                parent()->removeBuffer(_buffer);
        }
        _buffer = buffer;
        if (_buffer) {
            _buffer->attachView(this);
            onBufferStateChanged(StateChangeContext(), _buffer->state());
            conn(_buffer, SIGNAL(stateChanged(const StateChangeContext&, const BufferState&)),
                this, SLOT(onBufferStateChanged(const StateChangeContext&, const BufferState&)));

            reloadTitle();
            conn(_buffer, SIGNAL(nameChanged(const QString&)),
                this, SLOT(reloadTitle()));
            conn(_buffer, SIGNAL(nameChanged(const QString&)),
                this, SIGNAL(bufferNameChanged(const QString&)));

            conn(_buffer, SIGNAL(dirtyChanged(bool)),
                this, SLOT(reloadTitle()));
            conn(_buffer, SIGNAL(dirtyChanged(bool)),
                this, SIGNAL(bufferDirtyChanged(bool)));

            onBufferFiletypeChanged(_buffer->filetype(), false);
            conn(_buffer, SIGNAL(filetypeChanged(const QString&)),
                this, SLOT(onBufferFiletypeChanged(const QString&)));

            conn(_buffer, SIGNAL(progressChanged(float, bb::cascades::ProgressIndicatorState::Type, const QString&)),
                this, SLOT(onBufferProgressChanged(float, bb::cascades::ProgressIndicatorState::Type, const QString&)));

            conn(_buffer, SIGNAL(savedToFile(const QString&)),
                this, SLOT(onBufferSavedToFile(const QString&)));

            conn(_buffer, SIGNAL(filepathChanged(const QString&)),
                this, SIGNAL(bufferFilepathChanged(const QString&)));

            conn(_buffer, SIGNAL(lockedChanged(bool)),
                this, SIGNAL(bufferLockedChanged(bool)));

            conn(this, SIGNAL(undo()), _buffer, SLOT(undo()));
            conn(this, SIGNAL(redo()), _buffer, SLOT(redo()));
            conn(_buffer, SIGNAL(hasUndosChanged(bool)),
                this, SIGNAL(hasUndosChanged(bool)));
            conn(_buffer, SIGNAL(hasRedosChanged(bool)),
                this, SIGNAL(hasRedosChanged(bool)));
        }
    }
}

void View::setHighlightRangeLimit(int limit)
{
    if (limit != _highlightRangeLimit) {
        qDebug() << "setting highlightRangeLimit to" << _highlightRangeLimit;
        _highlightRangeLimit = limit;
        updateTextAreaPartialHighlight();
    }
}

void View::handleTextControlBasicModifiedKeys(bb::cascades::TextEditor *editor, bb::cascades::KeyEvent *event)
{
    switch (event->keycap()) {
        case KEYCODE_V: {
            bb::system::Clipboard clipboard;
            QString paste = clipboard.value("text/plain");
            editor->insertPlainText(paste);
            break;
        }
        case KEYCODE_SPACE:
            // TODO: this is kinda of a hack
            ((bb::cascades::AbstractTextControl *) editor->parent())->loseFocus();
            break;
    }
}

void View::onTextAreaModKeyPressed(bb::cascades::KeyEvent *event)
{
    _textArea->editor()->insertPlainText(event->unicode());
}

void View::onTextAreaModifiedKeyPressed(bb::cascades::KeyEvent *event, ModKeyListener *listener)
{
    switch (event->keycap()) {
        case KEYCODE_T: // Tab
            _textArea->editor()->insertPlainText("\t");
            break;
        case KEYCODE_Z: // Z on most platforms
            _buffer->undo();
            break;
        case KEYCODE_Y: // Redo
            _buffer->redo();
            break;
        case KEYCODE_H: // Head
            _mode->autoFocus(true);
            break;
        case KEYCODE_F: // Find
            setFindMode();
            break;
        case KEYCODE_S: // Save
            if (save() == OpenedFilePicker)
                listener->modOff();
            break;
        case KEYCODE_E: // Edit
            open(); // this always opens some sort of UI
            listener->modOff();
            break;
        case KEYCODE_D: // Delete
            killCurrentLine();
            break;
        case KEYCODE_X: // Kill
            close();
            break;
        case KEYCODE_G: // Germinate
            clone();
            break;
        case KEYCODE_C: // Create
            parent()->addNewView();
            break;
        case KEYCODE_Q:
            parent()->setPrevViewActive();
            break;
        case KEYCODE_W:
            parent()->setNextViewActive();
            break;
        default:
            handleTextControlBasicModifiedKeys(_textArea->editor(), event);
    }
}

void View::scrollTo(int cursorPosition)
{
    SignalBlocker blocker(_textArea);
    _textArea->editor()->setCursorPosition(cursorPosition);
    updateTextAreaPartialHighlight();
    _textArea->requestFocus();
    _textArea->loseFocus();
}

void View::scrollByLine(int offset)
{
    if (_buffer->state().empty() || _buffer->state().size() == 1)
        return;
    BufferState::Position focus = _buffer->state().focus(_textArea->editor()->cursorPosition());
    int to = qMin(qMax(focus.lineIndex+offset, 0), _buffer->state().size()-1);
    scrollTo(_buffer->state().cursorPositionAtLine(to) +
            qMin(focus.linePosition, _buffer->state()[to].line.size()));
}

void View::oneLineUp()
{
    scrollByLine(-1);
}

void View::oneLineDown()
{
    scrollByLine(1);
}

void View::scrollUp()
{
    scrollByLine(-SCROLL_RANGE);
}

void View::scrollDown()
{
    scrollByLine(SCROLL_RANGE);
}

void View::scrollToTop()
{
    if (!_buffer->state().empty())
        scrollTo(0);
}

void View::scrollToBottom()
{
    if (!_buffer->state().empty())
        scrollTo(_buffer->state().plainTextSize());
}

void View::killCurrentLine()
{
    _buffer->killLine(this, _textArea->editor()->cursorPosition());
}

void View::onTextAreaTextChanged(const QString& text)
{
    qDebug() << "## parsing change from text area";
//    qDebug() << "lines from the highlightStart:" <<
//            text.right(text.size() - _highlightStart.htmlCount + 1).left(100);
    _buffer->parseChange(this, text, _highlightStart, _textArea->editor()->cursorPosition());
}

Range View::partialHighlightRange(const BufferState &st, Range focus)
{
    // NOTE: the minium range would be (0,1)
    focus.grow(_highlightRangeLimit).clamp(0, qMax(st.size(), 1));
    // extend the range to make sure that we have non-empty lines at two ends
    while (focus.from > 0 && st[focus.from].line.isEmpty()) {
        focus.from--;
    }
    while (focus.to > 0 && focus.to < st.size() && st[focus.to-1].line.isEmpty()) {
        focus.to++;
    }
    bool reachedLimit = focus.from == 0 && focus.to >= st.size();
    // smart expansion of range depending on the input size
    if (!st.empty() && !reachedLimit) {
        int diff = 0, size = st.plainTextSize();
        int diffLimit = DIFF_LIMIT(size);
        for (int i = focus.from; i < focus.to; i++) {
            diff += st[i].diff();
        }
        qDebug() << "recorded diff (base range):" << diff;
        qDebug() << "plain text size:" << size;
        qDebug() << "diff limit:" << diffLimit;
        qDebug() << "expanding the partial highlight range to take advantage of diff...";
        while (diff < diffLimit && !reachedLimit) {
            reachedLimit = true;
            if (focus.from > 0) {
                reachedLimit = false;
                focus.from--;
                diff += st[focus.from].diff();
            }
            if (focus.to < st.size()) {
                reachedLimit = false;
                diff += st[focus.to].diff();
                focus.to++;
            }
        }
        qDebug() << "expanded to diff of" << diff << ", range:" << focus;
    }
    return focus;
}

void View::updateTextAreaPartialHighlight()
{
    SignalBlocker blocker(_textArea);
    int start = _textArea->editor()->selectionStart();
    int end = _textArea->editor()->selectionEnd();
    int startLine = _buffer->state().focus(start).lineIndex;
    int endLine = start == end ? startLine : _buffer->state().focus(end).lineIndex;
    // this is the case where we've just moved cursor to another place
    Range newRange = partialHighlightRange(_buffer->state(), Range(startLine, endLine));
    if (newRange != _highlightRange) {
        qDebug() << "## updating new partial highlight view from" << _highlightRange << "to" << newRange;
        _highlightRange = newRange;
        QString highlightedHtml;
        QTextStream output(&highlightedHtml);
        _highlightStart = _buffer->state().writeHighlightedHtml(output, _highlightRange);
        output.flush();
        qDebug() << "updated highlightStart to" << _highlightStart;
//        qDebug() << "lines from the highlightStart:" <<
//                highlightedHtml.right(highlightedHtml.size() - _highlightStart.htmlCount + 1).left(100);
//        qDebug() << "entire doc:" << highlightedHtml;
        _textArea->setText(highlightedHtml);
        _textArea->editor()->setSelection(start, end);
    }
}

void View::onTextAreaCursorPositionChanged()
{
    if (_buffer->filetype().isEmpty()) {
        return;
    }
    if (!_partialHighlightUpdateTimer.isActive())
        _partialHighlightUpdateTimer.start();
}

void View::onBufferFiletypeChanged(const QString &filetype, bool toast)
{
    if (filetype.isEmpty()) {
        setImageSource(QUrl("asset:///images/filetype/_blank.png"));
    } else {
        setImageSource(QUrl("asset:///images/filetype/"+filetype+".png"));
        if (toast)
            Utility::toast(tr("Filetype set to %1").arg(filetype));
    }
}

// Is textChanging or cursorPositionChanged emitted first?
void View::onBufferStateChanged(const StateChangeContext &ctx, const BufferState &state)
{
    if (this != ctx.sourceView || ctx.sourceViewShouldUpdate) {
        int pos = ctx.shouldMatchCursorPosition || this == ctx.sourceView ?
                state.cursorPosition() :
                _textArea->editor()->cursorPosition();
        // we assume that selectionStart == selectionEnd == pos in this case
        SignalBlocker blocker(_textArea);
        _highlightRange = partialHighlightRange(state, Range(state.focus(pos).lineIndex));
        QString highlightedHtml;
        QTextStream output(&highlightedHtml);
        _highlightStart = state.writeHighlightedHtml(output, _highlightRange);
        output.flush();

        qDebug() << "### text area:";
        qDebug() << _textArea->text();
        qDebug() << "### buffer:";
        qDebug() << highlightedHtml;

        _textArea->setText(highlightedHtml);
        _textArea->editor()->setCursorPosition(pos);
    }
}

void View::onBufferProgressChanged(float progress, bb::cascades::ProgressIndicatorState::Type state, const QString &msg)
{
    qDebug() << "received progress change" << progress;
    _progressIndicator->setState(state);
    _progressIndicator->setValue(progress);
    _progressIndicator->setVisible(progress > 0 && progress < 1);
    if (!msg.isNull())
        Utility::toast(msg, tr("OK"), this, SLOT(onProgressMessageDismissed(bb::system::SystemUiResult::Type)));
}

void View::onProgressMessageDismissed(bb::system::SystemUiResult::Type)
{
    _progressIndicator->setVisible(false);
}

void View::onTranslatorChanged()
{
    _textArea->setHintText(tr("Enter the content"));
    emit translatorChanged();
}

/* file related operations */
bb::cascades::pickers::FilePicker *View::filePicker()
{
    if (!_fpicker) {
        _fpicker = new bb::cascades::pickers::FilePicker(this);
        _fpicker->setType(bb::cascades::pickers::FileType::Document);
        _fpicker->setFilter(QStringList("*"));
        conn(_fpicker, SIGNAL(fileSelected(const QStringList&)),
                this, SLOT(onFileSelected(const QStringList&)));
    }
    if (_buffer->filepath().isEmpty()) {
        _fpicker->setDirectories(QStringList("/accounts/1000/shared"));
    } else {
        _fpicker->setDirectories(QStringList(QFileInfo(_buffer->filepath()).absolutePath()));
    }
    return _fpicker;
}

View::SaveStatus View::save()
{
    if (!_buffer->filepath().isEmpty()) {
        _buffer->save(_buffer->filepath());
        return Finished;
    }
    return saveAs();
}

View::SaveStatus View::saveAs()
{
    filePicker()->setMode(bb::cascades::pickers::FilePickerMode::Saver);
    filePicker()->setDefaultSaveFileNames(QStringList(_buffer->name()));
    filePicker()->setTitle(tr("Save"));
    filePicker()->open();
    return OpenedFilePicker;
}

void View::onBufferSavedToFile(const QString &)
{
    Utility::toast(tr("Saved"));
}

void View::open()
{
    if  (_buffer->views().size() == 1 && _buffer->dirty()) {
        Utility::dialog(tr("Yes"), tr("No"), tr("Unsaved change detected"),
                tr("Do you want to continue?"),
                this, SLOT(onUnsavedChangeDialogFinishedWhenOpening(bb::system::SystemUiResult::Type)));
    } else {
        pickFileToOpen();
    }
}

void View::onUnsavedChangeDialogFinishedWhenOpening(bb::system::SystemUiResult::Type type)
{
    if (type == bb::system::SystemUiResult::ConfirmButtonSelection) {
        pickFileToOpen();
    }
}

void View::pickFileToOpen()
{
    filePicker()->setMode(bb::cascades::pickers::FilePickerMode::Picker);
    filePicker()->setTitle(tr("Open"));
    filePicker()->open();
}

void View::onFileSelected(const QStringList &files)
{
    switch (filePicker()->mode()) {
        case bb::cascades::pickers::FilePickerMode::Picker:
            if (files[0] != _buffer->filepath()) {
                Buffer *b =parent()->bufferForFilepath(files[0]);
                if (b) {
                    setBuffer(b);
                } else {
                    if (_buffer->views().size() != 1) {
                        // not only bound to this view!
                        setBuffer(parent()->newBuffer());
                    }
                    _buffer->load(files[0]);
                }
            }
            break;
        case bb::cascades::pickers::FilePickerMode::Saver:
            _buffer->save(files[0]);
            break;
    }
}

void View::clone()
{
    parent()->cloneActive();
}

void View::close()
{
    if  (_buffer->views().size() == 1 && _buffer->dirty()) {
        Utility::dialog(tr("Yes"), tr("No"), tr("Unsaved change detected"),
                tr("Do you want to continue?"),
                this, SLOT(onUnsavedChangeDialogFinishedWhenClosing(bb::system::SystemUiResult::Type)));
    } else {
        parent()->remove(this);
    }
}

void View::onUnsavedChangeDialogFinishedWhenClosing(bb::system::SystemUiResult::Type type)
{
    if (type == bb::system::SystemUiResult::ConfirmButtonSelection) {
        parent()->remove(this);
    }
}
