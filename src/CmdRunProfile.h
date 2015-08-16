/*
 * CmdRunProfile.h
 *
 *  Created on: May 9, 2015
 *      Author: lingnan
 */

#ifndef CMDRUNPROFILE_H_
#define CMDRUNPROFILE_H_

#include <QProcess>
#include <RunProfile.h>

namespace bb {
    namespace cascades {
        class Page;
        class ActionItem;
        class Label;
        class TitleBar;
    }
}

class CmdRunProfile : public RunProfile
{
    Q_OBJECT
public:
    // cmd is a format string where
    // %1: the filepath of the file to run
    // %2: the directory the file is in
    // %3: the filename of the file
    CmdRunProfile(View *, const QString &cmd);
    virtual ~CmdRunProfile() {}
    void run();
    Q_SLOT void rerun();
    Q_SLOT void setCmd(const QString &cmd);
    bool runnable() const;
    void exit();
private:
    QString _cmd;
    bool _runnable;
    QProcess _process;

    bb::cascades::TitleBar *_titleBar;
    bb::cascades::ActionItem *_terminateAction;
    bb::cascades::ActionItem *_killAction;
    bb::cascades::ActionItem *_rerunAction;
    bb::cascades::ActionItem *_backButton;
    bb::cascades::Label *_outputArea;
    bb::cascades::Page *_outputPage;

    Q_SLOT void recalcRunnable();

    Q_SLOT void onNewStandardOutput();
    Q_SLOT void onNewStandardError();

    Q_SLOT void onViewPagePopped(bb::cascades::Page *);

    Q_SLOT void onShouldHideActionBarChanged(bool);
    Q_SLOT void onShouldHideTitleBarChanged(bool);

    Q_SLOT void onProcessStateChanged(QProcess::ProcessState);
    Q_SLOT void onTranslatorChanged();
};

#endif /* CMDRUNPROFILE_H_ */
