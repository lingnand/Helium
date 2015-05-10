/*
 * NormalMode.h
 *
 *  Created on: May 3, 2015
 *      Author: lingnan
 */

#ifndef NORMALMODE_H_
#define NORMALMODE_H_

#include <ViewMode.h>

namespace bb {
    namespace cascades {
        class TextField;
        class TitleBar;
        class ActionItem;
        class KeyEvent;
    }
}

class View;
class ModKeyListener;
class RunProfile;

class NormalMode: public ViewMode
{
    Q_OBJECT
public:
    NormalMode(View *);
    void autoFocus();
    void onEnter();
    void onExit();
    bb::cascades::TitleBar *titleBar() const;
    bb::cascades::TextField *titleField() const;
private:
    bool _lastFocused;

    bb::cascades::TitleBar *_titleBar;
    bb::cascades::TextField *_titleField;

    ModKeyListener *_textAreaModKeyListener;

    bb::cascades::ActionItem *_saveAction;
    bb::cascades::ActionItem *_saveAsAction;
    bb::cascades::ActionItem *_openAction;
    bb::cascades::ActionItem *_undoAction;
    bb::cascades::ActionItem *_redoAction;
    bb::cascades::ActionItem *_findAction;
    bb::cascades::ActionItem *_runAction;
    bb::cascades::ActionItem *_cloneAction;
    bb::cascades::ActionItem *_closeAction;

    Q_SLOT void reloadLocked();

    /** run profile **/
    RunProfile *_runProfile;
    void setRunProfile(RunProfile *);
    Q_SLOT void reloadRunnable();
    Q_SLOT void run();

    Q_SLOT void onTextAreaModKeyPressed(bb::cascades::KeyEvent *event);
    Q_SLOT void onTextAreaModifiedKeyPressed(bb::cascades::KeyEvent *event, ModKeyListener *listener);
    Q_SLOT void onTitleFieldFocusChanged(bool focus);
    Q_SLOT void onTitleFieldModifiedKeyPressed(bb::cascades::KeyEvent *event, ModKeyListener *listener);
    Q_SLOT void onBufferDirtyChanged(bool dirty);
    Q_SLOT void onBufferNameChanged(const QString &);
    Q_SLOT void onBufferFilepathChanged(const QString &);
    Q_SLOT void onBufferFiletypeChanged(const QString &);

    Q_SLOT void onTranslatorChanged();
};

#endif /* NORMALMODE_H_ */
