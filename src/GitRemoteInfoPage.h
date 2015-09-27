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

class GitRepoPage;

class GitRemoteInfoPage : public PushablePage
{
    Q_OBJECT
public:
    GitRemoteInfoPage(GitRepoPage *);
    void resetRemote();
    enum Mode { DisplayRemote, SaveRemote, Clone };
    void setMode(Mode=DisplayRemote, LibQGit2::Remote *remote=NULL);
    Q_SLOT void onTranslatorChanged();
Q_SIGNALS:
    void translatorChanged();
private:
    Mode _mode;
    GitRepoPage *_repoPage;
    LibQGit2::Remote *_remote;
    bb::cascades::Header *_nameHeader;
    bb::cascades::TextField *_nameField;
    bb::cascades::Header *_urlHeader;
    bb::cascades::TextField *_urlField;
    bb::cascades::ActionItem *_saveRemoteAction, *_cloneAction;
    Q_SLOT void saveRemote();
    Q_SLOT void clone();
    Q_SLOT void setRemoteUrl(const QString &);
};

#endif /* GITREMOTEINFOPAGE_H_ */
