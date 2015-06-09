/*
 * View.cpp
 *
 *  Created on: Nov 21, 2014
 *      Author: lingnan
 */

#include <bb/cascades/Page>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/Container>
#include <bb/cascades/ProgressIndicator>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/TextArea>
#include <bb/cascades/KeyEvent>
#include <bb/cascades/KeyListener>
#include <bb/cascades/Shortcut>
#include <bb/cascades/pickers/FilePicker>
#include <View.h>
#include <MultiViewPane.h>
#include <Helium.h>
#include <GeneralSettings.h>
#include <AppearanceSettings.h>
#include <Buffer.h>
#include <SignalBlocker.h>
#include <NormalMode.h>
#include <FindMode.h>
#include <Filetype.h>
#include <HighlightType.h>
#include <Utility.h>
#include <SettingsPage.h>
#include <ShortcutHelp.h>

using namespace bb::cascades;

#define SCROLL_RANGE 25

#define USE_SMART_HIGHLIGHT_RANGE 0
// totalResource = plainTextSize + c * diffLimit
// we are now assuming c to be 5
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
    _progressIndicator(ProgressIndicator::create()
        .visible(false)
        .vertical(VerticalAlignment::Bottom)
        .topMargin(0)),
    _pageKeyListener(KeyListener::create()
        .onKeyPressed(this, SLOT(onPageKeyPressed(bb::cascades::KeyEvent *))))
{
    GeneralSettings *general = Helium::instance()->general();
    _highlightRangeLimit = general->highlightRange();
    conn(general, SIGNAL(highlightRangeChanged(int)),
        this, SLOT(setHighlightRangeLimit(int)));

    conn(_textArea->editor(), SIGNAL(cursorPositionChanged(int)),
        this, SLOT(onTextAreaCursorPositionChanged()));
    conn(_textArea, SIGNAL(textChanging(const QString)),
        this, SLOT(onTextAreaTextChanged(const QString&)));

    // setup the timer for partial highlight update
    _partialHighlightUpdateTimer.setSingleShot(true);
    conn(&_partialHighlightUpdateTimer, SIGNAL(timeout()),
        this, SLOT(updateTextAreaPartialHighlight()));

    // initialize the pageKeyListener
    QVariantList helps;
    helps << QVariant::fromValue(ShortcutHelp("T", tr("Scroll to Top")))
          << QVariant::fromValue(ShortcutHelp("B", tr("Scroll to Bottom")))
          << QVariant::fromValue(ShortcutHelp(SPACE_SYMBOL, tr("One page down")))
          << QVariant::fromValue(ShortcutHelp(QString("%1 %2").arg(SHIFT_SYMBOL, SPACE_SYMBOL), tr("One page up")))
          << QVariant::fromValue(ShortcutHelp("U", tr("One page up")))
          << QVariant::fromValue(ShortcutHelp("J", tr("One line up")))
          << QVariant::fromValue(ShortcutHelp("K", tr("One line down")));
    _pageKeyListener->setProperty("shortcut_helps", helps);

    Shortcut *focus = Shortcut::create().key("Enter")
        .onTriggered(this, SLOT(autoFocus()));
    focus->setProperty("help", tr("Focus Editable Area"));
    _page = Page::create()
        .content(Container::create()
            .add(_textArea)
            .add(_progressIndicator))
//        .actionBarVisibility(ChromeVisibility::Visible)
//        .actionBarVisibility(ChromeVisibility::Hidden)
        .addShortcut(focus)
        // navigation
        .addKeyListener(_pageKeyListener);

    AppearanceSettings *appearance = Helium::instance()->appearance();
    onHideActionBarChanged(appearance->hideActionBar());
    conn(appearance, SIGNAL(hideActionBarChanged(bool)),
        this, SLOT(onHideActionBarChanged(bool)));

    setContent(_content = NavigationPane::create().add(_page));

    setBuffer(buffer);

    onTranslatorChanged();
    setMode(_normalMode = new NormalMode(this));
}

void View::onHideActionBarChanged(bool hide)
{
    _page->setActionBarVisibility(hide ?
            ChromeVisibility::Hidden :
            ChromeVisibility::Default);
}

void View::setMode(ViewMode *mode)
{
    if (mode != _mode) {
        if (_mode) {
            _mode->onExit();
        }
        _mode = mode;
        if (_mode) {
            _mode->onEnter();
        }
    }
}

// detachPage can only be called when at the main page
void View::detachContent()
{
    _content->setParent(NULL);
}

void View::reattachContent()
{
    _content->setParent(NULL);
    setContent(_content);
}

void View::hideAllPageActions()
{
    while (page()->actionCount() > 0) {
        page()->removeAction(page()->actionAt(0));
    }
}

bool View::active() const
{
    return parent() && parent()->activeTab() == this;
}

void View::onOutOfView()
{
    _textArea->loseFocus();
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

            emit bufferNameChanged(_buffer->name());
            conn(_buffer, SIGNAL(nameChanged(const QString&)),
                this, SIGNAL(bufferNameChanged(const QString&)));

            conn(_buffer, SIGNAL(dirtyChanged(bool)),
                this, SLOT(reloadTitle()));

            emit bufferDirtyChanged(_buffer->dirty());
            conn(_buffer, SIGNAL(dirtyChanged(bool)),
                this, SIGNAL(bufferDirtyChanged(bool)));

            onBufferFiletypeChanged(_buffer->filetype());
            conn(_buffer, SIGNAL(filetypeChanged(Filetype*, Filetype*)),
                this, SLOT(onBufferFiletypeChanged(Filetype*, Filetype*)));

            conn(_buffer, SIGNAL(progressChanged(float, bb::cascades::ProgressIndicatorState::Type, const QString&)),
                this, SLOT(onBufferProgressChanged(float, bb::cascades::ProgressIndicatorState::Type, const QString&)));

            conn(_buffer, SIGNAL(savedToFile(const QString&)),
                this, SLOT(onBufferSavedToFile(const QString&)));

            emit bufferFilepathChanged(_buffer->filepath());
            conn(_buffer, SIGNAL(filepathChanged(const QString&)),
                this, SIGNAL(bufferFilepathChanged(const QString&)));

            emit bufferLockedChanged(_buffer->locked());
            conn(_buffer, SIGNAL(lockedChanged(bool)),
                this, SIGNAL(bufferLockedChanged(bool)));

            emit bufferAutodetectFiletypeChanged(_buffer->autodetectFiletype());
            conn(_buffer, SIGNAL(autodetectFiletypeChanged(bool)),
                this, SIGNAL(bufferAutodetectFiletypeChanged(bool)));

            conn(this, SIGNAL(undo()), _buffer, SLOT(undo()));
            emit hasUndosChanged(_buffer->hasUndo());
            conn(_buffer, SIGNAL(hasUndosChanged(bool)),
                this, SIGNAL(hasUndosChanged(bool)));

            conn(this, SIGNAL(redo()), _buffer, SLOT(redo()));
            emit hasRedosChanged(_buffer->hasRedo());
            conn(_buffer, SIGNAL(hasRedosChanged(bool)),
                this, SIGNAL(hasRedosChanged(bool)));
        }
    }
}

void View::setHighlightRangeLimit(int limit)
{
    if (limit != _highlightRangeLimit) {
        _highlightRangeLimit = limit;
        qDebug() << "setting highlightRangeLimit to" << _highlightRangeLimit;
        updateTextAreaPartialHighlight();
    }
}

void View::blockPageKeyListener(bool block)
{
    _pageKeyListener->blockSignals(block);
}

void View::onPageKeyPressed(KeyEvent *event)
{
    switch (event->keycap()) {
        case KEYCODE_J:
            scrollByLine(1);
            break;
        case KEYCODE_K:
            scrollByLine(-1);
            break;
        case KEYCODE_T:
            if (!_buffer->state().empty())
                scrollTo(0);
            break;
        case KEYCODE_B:
            if (!_buffer->state().empty())
                scrollTo(INT_MAX);
            break;
        case KEYCODE_SPACE:
            if (event->isShiftPressed()) {
                scrollByLine(-SCROLL_RANGE);
            } else {
                scrollByLine(SCROLL_RANGE);
            }
            break;
        case KEYCODE_U:
            scrollByLine(-SCROLL_RANGE);
            break;
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
#if USE_SMART_HIGHLIGHT_RANGE
    reachedLimit = focus.from == 0 && focus.to >= st.size();
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
#endif
    return focus;
}

void View::updateTextAreaPartialHighlight()
{
    if (!_buffer->highlightType().shouldHighlight())
        return;
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
    if (!_buffer->highlightType().shouldHighlight())
        return;
    if (!_partialHighlightUpdateTimer.isActive())
        _partialHighlightUpdateTimer.start();
}

void View::onBufferFiletypeChanged(Filetype *change, Filetype *old)
{
    if (change) {
        setImageSource(QUrl("asset:///images/filetype/"+change->name()+".png"));
        if (active())
            Utility::toast(tr("Filetype set to %1").arg(change->name()));
    } else {
        setImageSource(QUrl("asset:///images/filetype/_blank.png"));
    }
    emit bufferFiletypeChanged(change, old);
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

void View::onBufferProgressChanged(float progress, ProgressIndicatorState::Type state, const QString &msg)
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
    _textArea->setHintText(tr("Content"));
    emit translatorChanged();
}

/* file related operations */
pickers::FilePicker *View::filePicker()
{
    if (!_fpicker) {
        _fpicker = new pickers::FilePicker(this);
        _fpicker->setType(pickers::FileType::Document);
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
    filePicker()->setMode(pickers::FilePickerMode::Saver);
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
    filePicker()->setMode(pickers::FilePickerMode::PickerMultiple);
    filePicker()->setTitle(tr("Open"));
    filePicker()->open();
}

void View::onFileSelected(const QStringList &files)
{
    switch (filePicker()->mode()) {
        case pickers::FilePickerMode::PickerMultiple: {
            int index = parent()->indexOf(this), i = 0;
            Buffer *b;
            for (; i < files.size()-1; i++) {
                b = parent() -> bufferForFilepath(files[i]);
                if (!b) {
                    b = parent()->newBuffer();
                    b->load(files[i]);
                }
                parent()->insertView(index+i, new View(b));
            }
            qDebug() << i << "views inserted during opening";
            if (i > 0) {
                Utility::toast(tr("%1 new views inserted").arg(i));
            }
            if (files[i] != _buffer->filepath()) {
                b =parent()->bufferForFilepath(files[i]);
                if (b) {
                    setBuffer(b);
                } else {
                    if (_buffer->views().size() != 1) {
                        // not only bound to this view!
                        setBuffer(parent()->newBuffer());
                    }
                    _buffer->load(files[i]);
                }
            }
            break;
        }
        case pickers::FilePickerMode::Saver:
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
        parent()->removeView(this);
    }
}

void View::onUnsavedChangeDialogFinishedWhenClosing(bb::system::SystemUiResult::Type type)
{
    if (type == bb::system::SystemUiResult::ConfirmButtonSelection) {
        parent()->removeView(this);
    }
}

void View::setAutodetectFiletype(bool autodetect)
{
    _buffer->setAutodetectFiletype(autodetect);
}
void View::setFiletype(Filetype *filetype)
{
    _buffer->setFiletype(filetype);
}

void View::setName(const QString &name)
{
    _buffer->setName(name);
}
