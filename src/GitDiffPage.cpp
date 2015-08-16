/*
 * GitDiffPage.cpp
 *
 *  Created on: Aug 13, 2015
 *      Author: lingnan
 */

#include <bb/cascades/ActionItem>
#include <bb/cascades/Shortcut>
#include <bb/cascades/Header>
#include <bb/cascades/Label>
#include <bb/cascades/TitleBar>
#include <bb/cascades/ScrollView>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/NavigationPane>
#include <srchilite/formattermanager.h>
#include <libqgit2/qgitrepository.h>
#include <libqgit2/qgitdifffile.h>
#include <GitRepoPage.h>
#include <GitDiffPage.h>
#include <Helium.h>
#include <AppearanceSettings.h>
#include <Utility.h>

using namespace bb::cascades;

GitDiffPage::GitDiffPage(GitRepoPage *page):
    _repoPage(page),
    _add(ActionItem::create()
        .addShortcut(Shortcut::create().key("a"))
        .onTriggered(this, SLOT(add()))),
    _reset(ActionItem::create()
        .addShortcut(Shortcut::create().key("r"))
        .onTriggered(this, SLOT(reset()))),
    _content(Segment::create()),
    _sourceHighlight("xhtml.outlang")
{
    setTitleBar(TitleBar::create());
    setContent(ScrollView::create(_content)
        .scrollMode(ScrollMode::Vertical));
    setActionBarVisibility(ChromeVisibility::Overlay);
    setActionBarAutoHideBehavior(ActionBarAutoHideBehavior::HideOnScroll);

    AppearanceSettings *appearance = Helium::instance()->appearance();
    onHighlightStyleFileChanged(appearance->highlightStyleFile());
    conn(appearance, SIGNAL(highlightStyleFileChanged(const QString&)),
        this, SLOT(onHighlightStyleFileChanged(const QString&)));

    onTranslatorChanged();
}

void GitDiffPage::onHighlightStyleFileChanged(const QString &styleFile)
{
    _sourceHighlight.setStyleFile(styleFile.toStdString());
    reloadContent();
}

void GitDiffPage::setPatch(const StatusPatch &spatch)
{
    _spatch = spatch;
    titleBar()->setTitle(_spatch.patch.delta().newFile().path());
    while (actionCount() > 0)
        removeAction(actionAt(0));
    switch (_spatch.type) {
        case HeadToIndex:
            addAction(_reset, ActionBarPlacement::Signature); break;
        case IndexToWorkdir:
            addAction(_add, ActionBarPlacement::Signature); break;
    }
    reloadContent();
}

void GitDiffPage::reloadContent()
{
    const LibQGit2::Patch &p = _spatch.patch;
    // reuse the children under content
    int i = 0;
    for (int numHunks = p.numHunks(); i < numHunks; i++) {
        GitDiffPage::HunkView *view;
        if (i == _content->count()) {
            view = new GitDiffPage::HunkView;
            _content->add(view);
        } else {
            view = (GitDiffPage::HunkView *) _content->at(i);
        }
        const LibQGit2::DiffHunk &hunk = p.hunk(i);
        view->header->setTitle(QString("-%1,%2 +%3,%4")
                .arg(hunk.oldStart()).arg(hunk.oldLines())
                .arg(hunk.newStart()).arg(hunk.newLines()));
        QString text;
        size_t numLines = hunk.numLines();
        if (numLines > 0) {
            QTextStream output(&text);
            size_t j = 0;
            while (true) {
                const LibQGit2::DiffLine &line = hunk.line(j);
                srchilite::FormatterPtr formatter;
                switch (line.type()) {
                    case LibQGit2::DiffLine::Addition:
                        formatter = _sourceHighlight.getFormatterManager()
                            ->getFormatter(std::string("diffadd"));
                        break;
                    case LibQGit2::DiffLine::Deletion:
                        formatter = _sourceHighlight.getFormatterManager()
                            ->getFormatter(std::string("diffdel"));
                        break;
                    case LibQGit2::DiffLine::Context:
                        formatter = _sourceHighlight.getFormatterManager()
                            ->getDefaultFormatter();
                        break;
                    default:
                        qDebug() << "UNHANDLED LINE" << QChar(line.type());
                        qDebug() << line.content();
                }
                if (formatter) {
                    QString content(QChar(line.type()));
                    content += line.content();
                    formatter->format(content.toStdString());
                    output << QString::fromStdString(_sourceHighlight.getBuffer().str());
                    _sourceHighlight.clearBuffer();
                }
                j++;
                if (j >= numLines)
                    break;
                if (line.numLines() == 0)
                    output << '\n';
            }
            output << flush;
        }
        view->text->setText(text);
    }
    // delete all the unused content
    for (; i < _content->count(); i++) {
        Control *con = _content->at(i);
        _content->remove(con);
        con->deleteLater();
    }
}

void GitDiffPage::resetPatch()
{
    _spatch = StatusPatch();
}

void GitDiffPage::add()
{
    _repoPage->addPaths(QList<QString>() << _spatch.patch.delta().newFile().path());
    parent()->pop();
}

void GitDiffPage::reset()
{
    _repoPage->resetPaths(QList<QString>() << _spatch.patch.delta().newFile().path());
    parent()->pop();
}

void GitDiffPage::onTranslatorChanged()
{
    PushablePage::onTranslatorChanged();
    _add->setTitle(tr("Add"));
    _reset->setTitle(tr("Reset"));
}

GitDiffPage::HunkView::HunkView():
    header(Header::create()),
    text(Label::create().multiline(true).format(TextFormat::Html)
        .textStyle(SystemDefaults::TextStyles::bodyText()))
{
    AppearanceSettings *appearance = Helium::instance()->appearance();
    text->textStyle()->setFontFamily(appearance->fontFamily());
    conn(appearance, SIGNAL(fontFamilyChanged(const QString&)),
        text->textStyle(), SLOT(setFontFamily(const QString&)));
    text->textStyle()->setFontSize(appearance->fontSize());
    conn(appearance, SIGNAL(fontSizeChanged(bb::cascades::FontSize::Type)),
        text->textStyle(), SLOT(setFontSize(bb::cascades::FontSize::Type)));

    add(header);
    add(Segment::create().subsection().add(text));
}
