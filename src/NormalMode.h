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

class NormalMode: public ViewMode
{
    Q_OBJECT
public:
    NormalMode(View *);
    void autoFocus(bool goToModeControl);
    Q_SLOT void setLocked(bool);
    void onEnter();
    void onExit();
    bb::cascades::TitleBar *titleBar() const;
    bb::cascades::TextField *titleField() const;
private:
    bool _lastFocused;

    bb::cascades::TitleBar *_titleBar;
    bb::cascades::TextField *_titleField;

    bb::cascades::ActionItem *_saveAction;
    bb::cascades::ActionItem *_saveAsAction;
    bb::cascades::ActionItem *_openAction;
    bb::cascades::ActionItem *_undoAction;
    bb::cascades::ActionItem *_redoAction;
    bb::cascades::ActionItem *_findAction;
    bb::cascades::ActionItem *_cloneAction;
    bb::cascades::ActionItem *_closeAction;

    Q_SLOT void onTitleFieldFocusChanged(bool focus);
    Q_SLOT void onTitleFieldModifiedKeyPressed(bb::cascades::KeyEvent *event, ModKeyListener *listener);
    Q_SLOT void onBufferDirtyChanged(bool dirty);
    Q_SLOT void onBufferNameChanged(const QString &);
    Q_SLOT void onBufferFilepathChanged(const QString &);

    Q_SLOT void onTranslatorChanged();
    void reloadActionTitles();
};

#endif /* NORMALMODE_H_ */
