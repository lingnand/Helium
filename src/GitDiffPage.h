/*
 * GitDiffPage.h
 *
 *  Created on: Aug 13, 2015
 *      Author: lingnan
 */

#ifndef GITDIFFPAGE_H_
#define GITDIFFPAGE_H_

#include <srchilite/sourcehighlight.h>
#include <libqgit2/qgitpatch.h>
#include <PushablePage.h>
#include <Segment.h>
#include <StatusActionSet.h>

namespace bb {
    namespace cascades {
        class ActionItem;
        class Header;
        class Label;
    }
}

class GitRepoPage;

struct StatusPatch {
    StatusDiffType type;
    LibQGit2::Patch patch;
    StatusPatch(StatusDiffType t=HeadToIndex, const LibQGit2::Patch &p=LibQGit2::Patch()):
        type(t), patch(p) {}
};

class GitDiffPage : public PushablePage
{
    Q_OBJECT
public:
    GitDiffPage(GitRepoPage *);
    void setPatch(const StatusPatch &);
    Q_SLOT void resetPatch(); // reset the patch
    void onTranslatorChanged();
private:
    GitRepoPage *_repoPage;
    bb::cascades::ActionItem *_add, *_reset;
    Segment *_content;
    StatusPatch _spatch;
    struct HunkView : public Segment {
        bb::cascades::Header *header;
        bb::cascades::Label *text;
        HunkView();
    };

    srchilite::SourceHighlight _sourceHighlight;

    Q_SLOT void add();
    Q_SLOT void reset();
    void reloadContent();

    Q_SLOT void onHighlightStyleFileChanged(const QString &);
};

#endif /* GITDIFFPAGE_H_ */
