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
#include <StatusActionSet.h>

namespace bb {
    namespace cascades {
        class ActionItem;
        class Header;
        class Label;
    }
}

class GitDiffPage : public PushablePage
{
    Q_OBJECT
public:
    GitDiffPage();
    void setPatch(const LibQGit2::Patch &);
    const LibQGit2::Patch &patch() const { return _patch; }
    Q_SLOT void resetPatch(); // reset the patch
    void hideAllActions();
private:
    Segment *_content;
    LibQGit2::Patch _patch;
    struct HunkView : public Segment {
        bb::cascades::Header *header;
        bb::cascades::Label *text;
        HunkView();
    };

    Q_SLOT void reloadContent();
};

#endif /* GITDIFFPAGE_H_ */
