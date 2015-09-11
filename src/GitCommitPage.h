/*
 * GitCommitPage.h
 *
 *  Created on: Aug 15, 2015
 *      Author: lingnan
 */

#ifndef GITCOMMITPAGE_H_
#define GITCOMMITPAGE_H_

#include <PushablePage.h>

namespace bb {
    namespace cascades {
        class ActionItem;
        class TextArea;
        class KeyEvent;
    }
}

class GitRepoPage;

class GitCommitPage : public PushablePage
{
    Q_OBJECT
public:
    GitCommitPage(GitRepoPage *);
    // the hint message in the commit box
    Q_SLOT void setHintMessage(const QString &);
    // focus the message field
    void focus();
    void onTranslatorChanged();
private:
    GitRepoPage *_repoPage;
    bb::cascades::TextArea *_textArea;
    bb::cascades::ActionItem *_settingsAction;
    Q_SLOT void commit();
    Q_SLOT void showSigSettings();
    Q_SLOT void onTextAreaModKey(bb::cascades::KeyEvent *event);
    Q_SLOT void onTextAreaModifiedKey(bb::cascades::KeyEvent *event);
};

#endif /* GITCOMMITPAGE_H_ */
