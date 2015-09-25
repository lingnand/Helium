/*
 * GitRemoteInfoPage.h
 *
 *  Created on: Sep 24, 2015
 *      Author: lingnan
 */

#ifndef GITREMOTEINFOPAGE_H_
#define GITREMOTEINFOPAGE_H_

#include <PushablePage.h>

namespace bb {
    namespace cascades {
        class Header;
        class TextField;
        class ActionItem;
    }
}

namespace LibQGit2 {
    class Remote;
}

class GitBranchPage;

class GitRemoteInfoPage : public PushablePage
{
    Q_OBJECT
public:
    GitRemoteInfoPage(GitBranchPage *);
    void setRemote(LibQGit2::Remote *);
    enum Action {
        SaveRemote = 1u << 0,
    };
    Q_DECLARE_FLAGS(Actions, Action)
    void setActions(Actions=Actions());
    void onTranslatorChanged();
Q_SIGNALS:
    void translatorChanged();
private:
    GitBranchPage *_branchPage;
    LibQGit2::Remote *_remote;
    bb::cascades::Header *_nameHeader;
    bb::cascades::TextField *_nameField;
    bb::cascades::Header *_urlHeader;
    bb::cascades::TextField *_urlField;
    bb::cascades::ActionItem *_saveRemoteAction;
    Q_SLOT void saveRemote();
    Q_SLOT void setRemoteUrl(const QString &);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GitRemoteInfoPage::Actions)

#endif /* GITREMOTEINFOPAGE_H_ */
