/*
 * GitDiffPage.h
 *
 *  Created on: Aug 13, 2015
 *      Author: lingnan
 */

#ifndef GITDIFFPAGE_H_
#define GITDIFFPAGE_H_

#include <libqgit2/qgitpatch.h>
#include <PushablePage.h>
#include <Segment.h>

namespace bb {
    namespace cascades {
        class ActionItem;
        class Header;
        class Label;
    }
}

class GitRepoPage;

class GitDiffPage : public PushablePage
{
    Q_OBJECT
public:
    GitDiffPage(GitRepoPage *);
    void setPatch(const LibQGit2::Patch &);
    Q_SLOT void resetPatch(); // reset the patch
    enum Action {
        Add = 1u << 0,
        Reset = 1u << 1,
    };
    Q_DECLARE_FLAGS(Actions, Action)
    void setActions(Actions=Actions());
    Q_SLOT void addFile();
    Q_SLOT void resetFile();
    void onTranslatorChanged();
Q_SIGNALS:
    void translatorChanged();
private:
    GitRepoPage *_repoPage;
    Segment *_content;
    bb::cascades::ActionItem *_addAction, *_resetAction;
    LibQGit2::Patch _patch;
    struct HunkView : public Segment {
        bb::cascades::Header *header;
        bb::cascades::Label *text;
        HunkView();
    };

    Q_SLOT void reloadContent();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GitDiffPage::Actions)

#endif /* GITDIFFPAGE_H_ */
