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
#include <libqgit2/qgitrepository.h>
#include <libqgit2/qgitdifffile.h>
#include <GitRepoPage.h>
#include <GitDiffPage.h>
#include <Utility.h>

using namespace bb::cascades;

GitDiffPage::GitDiffPage(GitRepoPage *page):
    RepushablePage(page),
    _repoPage(page),
    _add(ActionItem::create()
        .addShortcut(Shortcut::create().key("a"))
        .onTriggered(this, SLOT(add()))),
    _reset(ActionItem::create()
        .addShortcut(Shortcut::create().key("r"))
        .onTriggered(this, SLOT(reset()))),
    _content(Segment::create())
{
    setTitleBar(TitleBar::create());
    setContent(ScrollView::create(_content)
        .scrollMode(ScrollMode::Vertical));
    setActionBarVisibility(ChromeVisibility::Overlay);
    setActionBarAutoHideBehavior(ActionBarAutoHideBehavior::HideOnScroll);
    onTranslatorChanged();
}

void GitDiffPage::setPatch(const StatusPatch &spatch)
{
    _spatch = spatch;
    // refill the content
    const LibQGit2::Patch &p = _spatch.patch;
    titleBar()->setTitle(p.delta().newFile().path());
    while (actionCount() > 0)
        removeAction(actionAt(0));
    switch (_spatch.type) {
        case HeadToIndex:
            addAction(_reset, ActionBarPlacement::Signature); break;
        case IndexToWorkdir:
            addAction(_add, ActionBarPlacement::Signature); break;
    }
    // reuse the children under content
    int i = 0;
    qDebug() << "NUM HUNKS" << p.numHunks();
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
                QString prefix, affix;
                bool print = true;
                switch (line.type()) {
                    case LibQGit2::DiffLine::Addition:
                        prefix = "<span style='color:green'>";
                        affix = "</span>";
                        break;
                    case LibQGit2::DiffLine::Deletion:
                        prefix = "<span style='color:red'>";
                        affix = "</span>";
                        break;
                    case LibQGit2::DiffLine::Context:
                        break;
                    default:
                        qDebug() << "UNHANDLED LINE" << QChar(line.type());
                        qDebug() << line.content();
                        print = false;
                }
                if (print) {
                    output << prefix;
                    QString content(QChar(line.type()));
                    content += line.content();
                    QTextStream input(&content);
                    Utility::escapeHtml(input, output);
                    output << affix;
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
    for (; i < _content->count(); i++) {
        Control *con = _content->at(i);
        _content->remove(con);
        con->deleteLater();
    }
}

void GitDiffPage::add()
{
    _repoPage->addPaths(QList<QString>() << _spatch.patch.delta().newFile().path());
    pop();
}

void GitDiffPage::reset()
{
    _repoPage->resetPaths(QList<QString>() << _spatch.patch.delta().newFile().path());
    pop();
}

void GitDiffPage::onTranslatorChanged()
{
    _add->setTitle(tr("Add"));
    _reset->setTitle(tr("Reset"));
}

GitDiffPage::HunkView::HunkView():
    header(Header::create()),
    text(Label::create().multiline(true).format(TextFormat::Html)
        .textStyle(SystemDefaults::TextStyles::bodyText()))
{
    add(header);
    add(Segment::create().subsection().add(text));
}
