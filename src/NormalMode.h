/*
 * NormalMode.h
 *
 *  Created on: May 3, 2015
 *      Author: lingnan
 */

#ifndef NORMALMODE_H_
#define NORMALMODE_H_

#include <bb/cascades/Page>
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
class RunProfileManager;
class Filetype;
class FilePropertiesPage;

class NormalMode: public ViewMode
{
    Q_OBJECT
public:
    NormalMode(View *);
    virtual ~NormalMode() {}
    void autoFocus();
    void onEnter();
    void onExit();
private:
    bool _active;
    bool _lastFocused;
    bool _modifyingTextField;
    RunProfile *_runProfile;

    ModKeyListener *_textAreaModKeyListener;
    bb::cascades::ActionItem *_saveAction;
    bb::cascades::ActionItem *_saveAsAction;
    bb::cascades::ActionItem *_openAction;
    bb::cascades::ActionItem *_undoAction;
    bb::cascades::ActionItem *_redoAction;
    bb::cascades::ActionItem *_findAction;
    bb::cascades::ActionItem *_runAction;
    bb::cascades::ActionItem *_propertiesAction;
    bb::cascades::ActionItem *_cloneAction;
    bb::cascades::ActionItem *_renameAction;
    bb::cascades::ActionItem *_closeAction;
    bb::cascades::ActionItem *_closeProjectAction;
    FilePropertiesPage *_propertiesPage;

    bb::cascades::TitleBar *_titleBar;
    bb::cascades::TextField *_titleField;

    Q_SLOT void reloadLocked();

    void setRunProfile(RunProfile *);
    Q_SLOT void reloadRunnable();
    Q_SLOT void run();
    Q_SLOT void showProperties();

    Q_SLOT void resetTitleBar();
    Q_SLOT void rename();
    Q_SLOT void onTitleFieldTextChanged(const QString &);
    Q_SLOT void onTitleFieldFocusedChanged(bool);
    Q_SLOT void onTextAreaModKey(bb::cascades::KeyEvent *event);
    Q_SLOT void onTextAreaModifiedKey(bb::cascades::KeyEvent *event, ModKeyListener *listener);
    Q_SLOT void onTitleFieldModifiedKey(bb::cascades::KeyEvent *event, ModKeyListener *listener);
    Q_SLOT void onBufferDirtyChanged(bool dirty);
    Q_SLOT void onBufferNameChanged(const QString &);
    Q_SLOT void onBufferFilepathChanged(const QString &);
    Q_SLOT void onBufferFiletypeChanged(Filetype *change, Filetype *old=NULL);
    Q_SLOT void onRunProfileManagerChanged(RunProfileManager *);
    Q_SLOT void onTranslatorChanged();
};

#endif /* NORMALMODE_H_ */
