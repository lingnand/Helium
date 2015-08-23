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
#include <libqgit2/qgitrepository.h>
#include <libqgit2/qgitdifffile.h>
#include <GitDiffPage.h>
#include <Helium.h>
#include <AppearanceSettings.h>
#include <Utility.h>

using namespace bb::cascades;

GitDiffPage::GitDiffPage():
    _content(Segment::create())
{
    setTitleBar(TitleBar::create());
    setContent(ScrollView::create(_content)
        .scrollMode(ScrollMode::Vertical));
    setActionBarVisibility(ChromeVisibility::Overlay);
    setActionBarAutoHideBehavior(ActionBarAutoHideBehavior::HideOnScroll);

    Utility::connect(SIGNAL(htmlFormatterStyleChanged()),
            this, SLOT(reloadContent()));

    onTranslatorChanged();
}

void GitDiffPage::setPatch(const LibQGit2::Patch &patch)
{
    _patch = patch;
    titleBar()->setTitle(patch.delta().newFile().path());
    reloadContent();
}

void GitDiffPage::reloadContent()
{
    // reuse the children under content
    int i = 0;
    for (int numHunks = _patch.numHunks(); i < numHunks; i++) {
        GitDiffPage::HunkView *view;
        if (i == _content->count()) {
            view = new GitDiffPage::HunkView;
            _content->add(view);
        } else {
            view = (GitDiffPage::HunkView *) _content->at(i);
        }
        const LibQGit2::DiffHunk &hunk = _patch.hunk(i);
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
                QString elem;
                bool print = true;
                switch (line.type()) {
                    case LibQGit2::DiffLine::Addition:
                        elem = "diffadd"; break;
                    case LibQGit2::DiffLine::Deletion:
                        elem = "diffdel"; break;
                    case LibQGit2::DiffLine::Context:
                        break;
                    default:
                        print = false;
                        qDebug() << "UNHANDLED LINE" << QChar(line.type());
                        qDebug() << line.content();
                }
                if (print) {
                    QString content(QChar(line.type()));
                    content += line.content();
                    Utility::formatHtml(elem, content, output);
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
    _patch = LibQGit2::Patch();
}

void GitDiffPage::hideAllActions()
{
    while (actionCount() > 0)
        removeAction(actionAt(0));
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
