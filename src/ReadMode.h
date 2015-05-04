/*
 * ReadMode.h
 *
 *  Created on: May 4, 2015
 *      Author: lingnan
 */

#ifndef READMODE_H_
#define READMODE_H_

#include <ViewMode.h>

namespace bb {
    namespace cascades {
        class ActionItem;
    }
}

class NormalMode;

class ReadMode: public ViewMode
{
    Q_OBJECT
public:
    // share the controls in a normal mode
    ReadMode(NormalMode *);
    void autoFocus(bool goToModeControl);
    Q_SLOT void setLocked(bool);
    void onEnter(bool hasPreviousMode);
    void onExit();
private:
    NormalMode *_norm;

    bb::cascades::ActionItem *_editAction;
    bb::cascades::ActionItem *_findAction;
    bb::cascades::ActionItem *_openAction;
    bb::cascades::ActionItem *_cloneAction;
    bb::cascades::ActionItem *_closeAction;

    Q_SLOT void onTranslatorChanged();
    void reloadActionTitles();
};

#endif /* READMODE_H_ */
