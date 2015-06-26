/*
 * CmdRunProfile.cpp
 *
 *  Created on: May 9, 2015
 *      Author: lingnan
 */

#include <QFileInfo>
#include <bb/cascades/TitleBar>
#include <bb/cascades/Page>
#include <bb/cascades/ActionItem>
#include <bb/cascades/Shortcut>
#include <bb/cascades/Label>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/ScrollView>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/NavigationPaneProperties>
#include <CmdRunProfile.h>
#include <View.h>
#include <Buffer.h>
#include <Segment.h>
#include <Utility.h>

using namespace bb::cascades;

CmdRunProfile::CmdRunProfile(View *view, const QString &cmd):
    RunProfile(view), _cmd(cmd),
    _runnable(false),
    _killAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_cancel.png"))
        .addShortcut(Shortcut::create().key("k"))
        .onTriggered(&_process, SLOT(kill()))),
    _rerunAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_reload.png"))
        .addShortcut(Shortcut::create().key("r"))
        .onTriggered(this, SLOT(rerun()))),
    _outputArea(Label::create().multiline(true)
        .format(TextFormat::Html)
        .contentFlags(TextContentFlag::ActiveTextOff)
        .textStyle(SystemDefaults::TextStyles::bodyText())
        .preferredWidth(0))
{
    conn(view->content(), SIGNAL(pushTransitionEnded(bb::cascades::Page*)),
        this, SLOT(onViewPagePushed(bb::cascades::Page*)));
    conn(view->content(), SIGNAL(popTransitionEnded(bb::cascades::Page*)),
        this, SLOT(onViewPagePopped(bb::cascades::Page*)));
    conn(&_process, SIGNAL(readyReadStandardOutput()),
        this, SLOT(onNewStandardOutput()));
    conn(&_process, SIGNAL(readyReadStandardError()),
        this, SLOT(onNewStandardError()));

    _outputPage = Page::create()
        .titleBar(TitleBar::create())
        .content(ScrollView::create()
            .scrollMode(ScrollMode::Vertical)
                .content(Segment::create()
                    .section().subsection()
                    .add(_outputArea)))
        .actionBarVisibility(ChromeVisibility::Overlay)
        .addAction(_rerunAction, ActionBarPlacement::Signature)
        .addAction(_killAction, ActionBarPlacement::OnBar)
        .paneProperties(NavigationPaneProperties::create()
            .backButton(ActionItem::create()
                .addShortcut(Shortcut::create().key("x"))
                .onTriggered(view->content(), SLOT(pop()))));
    _outputPage->setActionBarAutoHideBehavior(ActionBarAutoHideBehavior::HideOnScroll);
    // when not activated _outputPage is owned by this profile,
    // so when the profile is replaced its associated view elements
    // are removed gracefully
    _outputPage->setParent(this);

    onTranslatorChanged();
    conn(view, SIGNAL(translatorChanged()), this, SLOT(onTranslatorChanged()));

    onProcessStateChanged(_process.state());
    conn(view, SIGNAL(bufferFilepathChanged(const QString&)),
        this, SLOT(recalcRunnable()));
    conn(&_process, SIGNAL(stateChanged(QProcess::ProcessState)),
        this, SLOT(onProcessStateChanged(QProcess::ProcessState)));
}

void CmdRunProfile::run()
{
    if (!_runnable) {
        Utility::toast(tr("Unable to run (Probably haven't saved)"));
        return;
    }
    _outputPage->setParent(NULL);
    view()->content()->push(_outputPage);
}

void CmdRunProfile::exit()
{
    _process.kill();
    _outputArea->resetText();
    // pop the page if necessary
    if (view()->content()->top() == _outputPage) {
        view()->content()->pop();
    }
    _outputPage->setParent(this);
}

QString formatCmd(const QString &format, const QString &path, const QString &dir, const QString &name)
{
    QString specifier;
    QString output;
    for (int i = 0; i < format.size(); i++) {
        if (format[i] == '%') {
            if (specifier == "%") {
                output += '%';
                specifier.clear();
            } else if (specifier == "%path") {
                output += path;
                specifier.clear();
            } else if (specifier == "%dir") {
                output += dir;
                specifier.clear();
            } else if (specifier == "%name") {
                output += name;
                specifier.clear();
            } else {
                specifier = "%";
            }
        } else if (!specifier.isEmpty()) {
            specifier += format[i];
        } else {
            output += format[i];
        }
    }
    return output;
}

void CmdRunProfile::rerun()
{
    // kill the process (if it's not already killed)
    _process.kill();
    // clear the view first
    _outputArea->resetText();
    QFileInfo f(view()->buffer()->filepath());
    QStringList args;
    args << "-c" << formatCmd(_cmd, f.absoluteFilePath(), f.absolutePath(), f.fileName());
    qDebug() << "Running command with args:" << args;
    _process.start("/bin/sh", args);
}

void CmdRunProfile::setCmd(const QString &cmd)
{
    if (cmd != _cmd) {
        _cmd = cmd;
    }
}

bool CmdRunProfile::runnable() const
{
    return _runnable;
}

void CmdRunProfile::recalcRunnable()
{
    bool nRunnable = _process.state() == QProcess::NotRunning
            && !view()->buffer()->filepath().isEmpty();
    if (nRunnable != _runnable) {
        _runnable = nRunnable;
        emit runnableChanged(_runnable);
    }
}

void CmdRunProfile::onProcessStateChanged(QProcess::ProcessState state)
{
    recalcRunnable();
    _killAction->setEnabled(state != QProcess::NotRunning);
}

void CmdRunProfile::onNewStandardOutput()
{
    QString raw = _process.readAllStandardOutput(), out;
    QTextStream sraw(&raw), sout(&out);
    sout << _outputArea->text();
    Utility::escapeHtml(sraw, sout);
    sout << flush;
    _outputArea->setText(out);
}

void CmdRunProfile::onNewStandardError()
{
    QString raw = _process.readAllStandardError(), out;
    QTextStream sraw(&raw), sout(&out);
    sout << _outputArea->text()
         << "<span style='color:red'>";
    Utility::escapeHtml(sraw, sout);
    sout << "</span>" << flush;
    _outputArea->setText(out);
}

void CmdRunProfile::onViewPagePushed(Page *page)
{
    if (page == _outputPage) {
        qDebug() << "output page activated";
        // run the actual command
        rerun();
    }
}

void CmdRunProfile::onViewPagePopped(Page *page)
{
    if (page == _outputPage) {
        qDebug() << "output page out of view";
        exit();
    }
}

void CmdRunProfile::onTranslatorChanged()
{
    _outputPage->titleBar()->setTitle(tr("Run"));
    _killAction->setTitle(tr("Kill"));
    _rerunAction->setTitle(tr("Rerun"));
}
