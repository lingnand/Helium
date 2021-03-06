/*
 * GitRepoPage.cpp
 *
 *  Created on: Aug 8, 2015
 *      Author: lingnan
 */

#include <bb/cascades/TitleBar>
#include <bb/cascades/MultiSelectActionItem>
#include <bb/cascades/ActionItem>
#include <bb/cascades/ActionSet>
#include <bb/cascades/Shortcut>
#include <bb/cascades/ScrollView>
#include <bb/cascades/ListView>
#include <bb/cascades/Label>
#include <bb/cascades/Header>
#include <bb/cascades/StandardListItem>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/Container>
#include <libqgit2/qgitexception.h>
#include <libqgit2/qgitdifffile.h>
#include <libqgit2/qgitdiff.h>
#include <libqgit2/qgitremote.h>
#include <Defaults.h>
#include <GitRepoPage.h>
#include <GitCommitPage.h>
#include <GitBranchPage.h>
#include <GitSettingsPage.h>
#include <Project.h>
#include <Segment.h>
#include <SignalBlocker.h>
#include <AutoHideProgressIndicator.h>
#include <LocaleAwareActionItem.h>
#include <Helium.h>
#include <GitSettings.h>
#include <Utility.h>

using namespace bb::cascades;

bool validDiffDelta(const LibQGit2::DiffDelta &delta)
{
    switch (delta.type()) {
        case LibQGit2::DiffDelta::Unknown:
        case LibQGit2::DiffDelta::Unmodified:
            return false;
    }
    return true;
}

GitRepoPage::GitRepoPage():
    _project(NULL),
    _contentHolder(Container::create()),
    _initAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_git_init.png"))
        .addShortcut(Shortcut::create().key("i"))
        .onTriggered(this, SLOT(init()))),
    _cloneAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_git_clone.png"))
        .addShortcut(Shortcut::create().key("c"))
        .onTriggered(this, SLOT(safePushRemoteInfoPageForClone()))),
    _reloadAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_reload.png"))
        .addShortcut(Shortcut::create().key("l"))
        .onTriggered(this, SLOT(reload()))),
    _settingsAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_settings.png"))
        .onTriggered(this, SLOT(pushSettingsPage()))),
    _noRepoLabel(Label::create().multiline(true)
        .format(TextFormat::Html)
        .preferredWidth(0)
        .contentFlags(TextContentFlag::ActiveTextOff)
        .textStyle(Defaults::centeredBodyText())),
    _noRepoContent(ScrollView::create(
            Segment::create().section().subsection()
                .add(_noRepoLabel))
        .scrollMode(ScrollMode::Vertical)
        .scrollRole(ScrollRole::Main)),
    _commitAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_git_commit.png"))
        .addShortcut(Shortcut::create().key("c"))
        .onTriggered(this, SLOT(pushCommitPage()))),
    _branchesAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_git_branch.png"))
        .addShortcut(Shortcut::create().key("h"))
        .onTriggered(this, SLOT(pushBranchPage()))),
    _logAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_git_log.png"))
        .addShortcut(Shortcut::create().key("g"))
        .onTriggered(this, SLOT(pushLogPage()))),
    _addAllAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_add.png"))
        .addShortcut(Shortcut::create().key("a"))
        .onTriggered(this, SLOT(addAll()))),
    _resetMixedAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_git_reset_mixed.png"))
        .addShortcut(Shortcut::create().key("r"))
        .onTriggered(this, SLOT(resetMixed()))),
    _resetHardAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_git_reset_hard.png"))
        .addShortcut(Shortcut::create().key("e"))
        .onTriggered(this, SLOT(safeResetHard()))),
    _statusItemProvider(this),
    _statusListView(ListView::create()
        .scrollRole(ScrollRole::Main)
        .dataModel(&_statusDataModel)
        .listItemProvider(&_statusItemProvider)),
    _repoContent(_statusListView),
    _progressIndicator(new AutoHideProgressIndicator),
    _multiAddAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_add.png"))
        .addShortcut(Shortcut::create().key("a"))
        .onTriggered(this, SLOT(addSelections()))),
    _multiResetAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_remove.png"))
        .addShortcut(Shortcut::create().key("r"))
        .onTriggered(this, SLOT(resetSelections()))),
    _rebaseNextAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_forward.png"))
        .addShortcut(Shortcut::create().key("n"))
        .onTriggered(this, SLOT(rebaseNext()))),
    _rebaseAbortAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_cancel.png"))
        .addShortcut(Shortcut::create().key("x"))
        .onTriggered(this, SLOT(rebaseAbort()))),
    _mergeAbortAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_cancel.png"))
        .addShortcut(Shortcut::create().key("x"))
        .onTriggered(this, SLOT(mergeAbort()))),
    _diffPage(NULL),
    _logPage(NULL),
    _commitPage(NULL),
    _commitInfoPage(NULL),
    _branchPage(NULL),
    _remoteInfoPage(NULL),
    _settingsPage(NULL),
    _tempRemote(NULL)
{
    _statusListView->setMultiSelectAction(MultiSelectActionItem::create());
    _statusListView->multiSelectHandler()->addAction(_multiAddAction);
    _statusListView->multiSelectHandler()->addAction(_multiResetAction);
    _statusListView->setEnabled(false);
    conn(_statusListView, SIGNAL(triggered(QVariantList)),
        this, SLOT(showDiffIndexPath(const QVariantList &)));
    conn(_statusListView, SIGNAL(selectionChangeStarted()),
        this, SLOT(reloadMultiSelectActionsEnabled()));
    setTitleBar(TitleBar::create());
    setContent(Container::create()
        .add(_contentHolder)
        .add(_progressIndicator));

    onTranslatorChanged(false);
}

LibQGit2::Repository *GitRepoPage::repo()
{
    return _project->gitRepo();
}

void GitRepoPage::setProject(Project *project)
{
    if (project != _project) {
        if (_project) {
            _project->disconnect(this);
            _project->gitWorker()->disconnect(this);
            disconnect(_project->gitWorker());
        }
        _project = project;
        if (_project) {
            conn(this, SIGNAL(workerFetchStatusList()),
                _project->gitWorker(), SLOT(fetchStatusList()));
            conn(this, SIGNAL(workerAddPaths(const QList<QString>&)),
                _project->gitWorker(), SLOT(addPaths(const QList<QString>&)));
            conn(this, SIGNAL(workerResetPaths(const QList<QString>&)),
                _project->gitWorker(), SLOT(resetPaths(const QList<QString>&)));
            conn(this, SIGNAL(workerRebase(const LibQGit2::Reference&)),
                _project->gitWorker(), SLOT(rebase(const LibQGit2::Reference&)));
            conn(this, SIGNAL(workerRebaseNext()),
                _project->gitWorker(), SLOT(rebaseNext()));
            conn(this, SIGNAL(workerRebaseAbort()),
                _project->gitWorker(), SLOT(rebaseAbort()));
            conn(this, SIGNAL(workerCommit(const QString&)),
                _project->gitWorker(), SLOT(commit(const QString&)));
            conn(this, SIGNAL(workerReset(LibQGit2::Repository::ResetType)),
                _project->gitWorker(), SLOT(reset(LibQGit2::Repository::ResetType)));
            conn(this, SIGNAL(workerCheckoutCommit(const LibQGit2::Object&)),
                _project->gitWorker(), SLOT(checkoutCommit(const LibQGit2::Object&)));
            conn(this, SIGNAL(workerCheckoutBranch(const LibQGit2::Reference&)),
                _project->gitWorker(), SLOT(checkoutBranch(const LibQGit2::Reference&)));
            conn(this, SIGNAL(workerMerge(const LibQGit2::Reference&)),
                _project->gitWorker(), SLOT(merge(const LibQGit2::Reference&)));
            conn(this, SIGNAL(workerCleanupState()),
                _project->gitWorker(), SLOT(cleanupState()));
            conn(this, SIGNAL(workerDeleteBranch(LibQGit2::Reference)),
                _project->gitWorker(), SLOT(deleteBranch(LibQGit2::Reference)));
            conn(this, SIGNAL(workerCreateBranch(const QString&)),
                _project->gitWorker(), SLOT(createBranch(const QString&)));
            conn(this, SIGNAL(workerFetch(LibQGit2::Remote*, const LibQGit2::Reference&)),
                _project->gitWorker(), SLOT(fetch(LibQGit2::Remote*, const LibQGit2::Reference&)));
            conn(this, SIGNAL(workerFetchBaseAndPrune(LibQGit2::Remote*)),
                _project->gitWorker(), SLOT(fetchBaseAndPrune(LibQGit2::Remote*)));
            conn(this, SIGNAL(workerPull(LibQGit2::Remote*, const LibQGit2::Reference&)),
                _project->gitWorker(), SLOT(pull(LibQGit2::Remote*, const LibQGit2::Reference&)));
            conn(this, SIGNAL(workerPush(LibQGit2::Remote*, const QString&)),
                _project->gitWorker(), SLOT(push(LibQGit2::Remote*, const QString&)));
            conn(this, SIGNAL(workerCreateRemote(const QString&, const QString&, const LibQGit2::Credentials&)),
                _project->gitWorker(), SLOT(createRemote(const QString&, const QString&, const LibQGit2::Credentials&)));
            conn(this, SIGNAL(workerClone(const QString&, const QString&, const LibQGit2::Credentials&)),
                _project->gitWorker(), SLOT(clone(const QString&, const QString&, const LibQGit2::Credentials&)));

            onGitWorkerInProgressChanged(_project->gitWorker()->inProgress());
            conn(_project->gitWorker(), SIGNAL(inProgressChanged(bool)),
                this, SLOT(onGitWorkerInProgressChanged(bool)));
            conn(_project->gitWorker(), SIGNAL(inProgressChanged(bool)),
                this, SIGNAL(inProgressChanged(bool)));
            // XXX: assume there is no progress right now
            _progressIndicator->hide();
            conn(_project->gitWorker(), SIGNAL(progressChanged(float, bb::cascades::ProgressIndicatorState::Type)),
                _progressIndicator, SLOT(displayProgress(float, bb::cascades::ProgressIndicatorState::Type)));
            conn(_project->gitWorker(), SIGNAL(progressChanged(float, bb::cascades::ProgressIndicatorState::Type)),
                this, SIGNAL(progressChanged(float, bb::cascades::ProgressIndicatorState::Type)));
            conn(_project->gitWorker(), SIGNAL(progressDismissed()),
                _progressIndicator, SLOT(hide()));
            conn(_project->gitWorker(), SIGNAL(progressDismissed()),
                this, SIGNAL(progressDismissed()));
            conn(_project->gitWorker(), SIGNAL(statusListFetched(const LibQGit2::StatusList&)),
                this, SLOT(handleStatusList(const LibQGit2::StatusList&)));
            conn(_project->gitWorker(), SIGNAL(pushCommitPage(const QString&)),
                this, SLOT(pushCommitPage(const QString&)));

            conn(_project, SIGNAL(pathChanged(const QString&)),
                this, SLOT(onProjectPathChanged()));

            if (!_project->gitRepo()->isNull())
                emit workerFetchStatusList();
        }
    }
}

void GitRepoPage::resetProject()
{
    setProject(NULL);
}

bool GitRepoPage::inProgress()
{
    return _project->gitWorker()->inProgress();
}

void GitRepoPage::reloadMultiSelectActionsEnabled()
{
    _multiAddAction->setEnabled(false);
    _multiResetAction->setEnabled(false);
    const QVariantList list = _statusListView->selectionList();
    if (list.empty())
        return;
    int header = list[0].toList()[0].toInt();
    for (int i = 1; i < list.size(); i++) {
        if (list[i].toList()[0].toInt() != header)
            return;
    }
    switch (header) {
        case 0:
            _multiResetAction->setEnabled(true); break;
        case 1:
            _multiAddAction->setEnabled(true); break;
    }
}

void GitRepoPage::hideAllActions()
{
    while (actionCount() > 0)
        removeAction(actionAt(0));
}

void GitRepoPage::handleStatusList(const LibQGit2::StatusList &list)
{
    _statusDataModel.setStatusList(list);
}

void GitRepoPage::rebaseNext()
{
    emit workerRebaseNext();
}

void GitRepoPage::rebaseAbort()
{
    emit workerRebaseAbort();
}

void GitRepoPage::reload()
{
    reloadContent();
    if (!_project->gitRepo()->isNull())
        emit workerFetchStatusList();
}

void GitRepoPage::reloadContent()
{
    if (_project->gitRepo()->isNull()) {
        // title
        titleBar()->setTitle(tr("No Repository"));
        // actions
        hideAllActions();
        _initAction->setEnabled(true);
        _cloneAction->setEnabled(true);
        _reloadAction->setEnabled(true);
        _settingsAction->setEnabled(true);
        addAction(_initAction, ActionBarPlacement::Signature);
        addAction(_cloneAction, ActionBarPlacement::OnBar);
        addAction(_reloadAction, ActionBarPlacement::OnBar);
        addAction(_settingsAction);
        // content
        _noRepoLabel->setText(tr("<br/>No repository found.<br/><br/>Use <b>Init</b> or <b>Clone</b> to create a git repository in<br/><em>%1</em>")
                .arg(_project->path()));
        _contentHolder->remove(_repoContent);
        _contentHolder->add(_noRepoContent);
        return;
    }
    // title
    QString title;
    if (_project->gitRepo()->isHeadUnborn()) {
        title = tr("No commit");
    } else if (_project->gitRepo()->isHeadDetached()) {
        title = tr("Detached at %1").arg(
            QString(_project->gitRepo()->head().peelToCommit().oid().nformat(7)));
    } else {
        title = _project->gitRepo()->head().branchName();
    }
    // actions
    hideAllActions();
    bool validDeltasInIndex = _statusDataModel.hasValidDiffDeltasInIndex();
    bool validDeltasInWorkdir = _statusDataModel.hasValidDiffDeltasInWorkdir();
    switch (_project->gitRepo()->state()) {
        case LibQGit2::Repository::StateNone: {
            _commitAction->setEnabled(validDeltasInIndex);
            _branchesAction->setEnabled(true);
            _logAction->setEnabled(!_project->gitRepo()->isHeadUnborn());
            addAction(_commitAction, ActionBarPlacement::Signature);
            addAction(_branchesAction, ActionBarPlacement::OnBar);
            addAction(_logAction, ActionBarPlacement::OnBar);
            break;
        }
        case LibQGit2::Repository::StateMerge:
            title += tr(" (merge in progress)");
            _commitAction->setEnabled(validDeltasInIndex);
            _mergeAbortAction->setEnabled(true);
            addAction(_commitAction, ActionBarPlacement::Signature);
            addAction(_mergeAbortAction, ActionBarPlacement::OnBar);
            break;
        case LibQGit2::Repository::StateRevert:
            title += tr(" (revert in progress)");
            break;
        case LibQGit2::Repository::StateCherrypick:
            title += tr(" (cherrypick in progress)");
            break;
        case LibQGit2::Repository::StateBisect:
            title += tr(" (bisect in progress)");
            break;
        case LibQGit2::Repository::StateRebaseMerge:
        case LibQGit2::Repository::StateRebase:
            title += tr(" (rebase in progress)");
            _rebaseNextAction->setEnabled(true);
            _rebaseAbortAction->setEnabled(true);
            addAction(_rebaseNextAction, ActionBarPlacement::Signature);
            addAction(_rebaseAbortAction, ActionBarPlacement::OnBar);
            break;
        case LibQGit2::Repository::StateInteractive:
            title += tr(" (interactive)");
            break;
        case LibQGit2::Repository::StateApplyMailbox:
            title += tr(" (apply mailbox)");
            break;
    }
    _addAllAction->setEnabled(validDeltasInWorkdir);
    _resetMixedAction->setEnabled(validDeltasInIndex);
    _resetHardAction->setEnabled(validDeltasInIndex || validDeltasInWorkdir);
    _reloadAction->setEnabled(true);
    _settingsAction->setEnabled(true);
    addAction(_addAllAction);
    addAction(_resetMixedAction);
    addAction(_resetHardAction);
    addAction(_reloadAction);
    addAction(_settingsAction);
    // title
    titleBar()->setTitle(title);
    // content
    _statusListView->setEnabled(true);
    _contentHolder->remove(_noRepoContent);
    _contentHolder->add(_repoContent);
}

void GitRepoPage::init()
{
    _project->gitRepo()->init(_project->path());
    emit workerFetchStatusList();
}

void GitRepoPage::safePushRemoteInfoPageForClone()
{
    // check if there are any files in the existing directory
    // if yes, prompt the user if they want to force clone
    if (!QDir(_project->path()).entryList(QDir::NoDotAndDotDot|QDir::AllEntries).empty())
        Utility::dialog(tr("Continue"), tr("Cancel"), tr("Confirm Clone"),
            tr("Project directory \"%1\" is not empty. Are you sure you want to clone into this directory?")
                .arg(Utility::shortenPath(_project->path())),
            this, SLOT(onCloneDialogFinished(bb::system::SystemUiResult::Type)));
    else
        pushRemoteInfoPage(GitRemoteInfoPage::Clone);
}

void GitRepoPage::onCloneDialogFinished(bb::system::SystemUiResult::Type type)
{
    if (type == bb::system::SystemUiResult::ConfirmButtonSelection)
        pushRemoteInfoPage(GitRemoteInfoPage::Clone);
}

void GitRepoPage::commit(const QString &message)
{
    emit workerCommit(message);
}

void GitRepoPage::checkoutCommit(const LibQGit2::Object &commit)
{
    emit workerCheckoutCommit(commit);
}

void GitRepoPage::checkoutBranch(const LibQGit2::Reference &branch)
{
    emit workerCheckoutBranch(branch);
}

void GitRepoPage::merge(const LibQGit2::Reference &theirHead)
{
    emit workerMerge(theirHead);
}

void GitRepoPage::mergeAbort()
{
    emit workerCleanupState();
}

void GitRepoPage::rebase(const LibQGit2::Reference &upstream)
{
    emit workerRebase(upstream);
}

void GitRepoPage::deleteBranch(const LibQGit2::Reference &branch)
{
    emit workerDeleteBranch(branch);
}

void GitRepoPage::safeDeleteBranch(const LibQGit2::Reference &branch)
{
    _tempTarget = branch;
    Utility::dialog(tr("Continue"), tr("Cancel"), tr("Confirm Deletion"),
            tr("Are you sure you want to delete branch %1?")
                .arg(_tempTarget.branchName()),
            this, SLOT(onDeleteBranchDialogFinished(bb::system::SystemUiResult::Type)));
}

void GitRepoPage::onDeleteBranchDialogFinished(bb::system::SystemUiResult::Type type)
{
    if (type == bb::system::SystemUiResult::ConfirmButtonSelection)
        deleteBranch(_tempTarget);
    _tempTarget = LibQGit2::Reference();
}

void GitRepoPage::createBranch(const QString &branchName)
{
    emit workerCreateBranch(branchName);
}

void GitRepoPage::fetch(LibQGit2::Remote *remote, const LibQGit2::Reference &branch)
{
    emit workerFetch(remote, branch);
}

void GitRepoPage::fetchBaseAndPrune(LibQGit2::Remote *remote)
{
    emit workerFetchBaseAndPrune(remote);
}

void GitRepoPage::pull(LibQGit2::Remote *remote, const LibQGit2::Reference &branch)
{
    emit workerPull(remote, branch);
}

void GitRepoPage::push(LibQGit2::Remote *remote, const QString &branch)
{
    emit workerPush(remote, branch);
}

void GitRepoPage::safePush(LibQGit2::Remote *remote, const QString &branch)
{
    // check if the current branch has the same name as the one to be pushed into
    // if not, ask for confirmation
    QString currentBranch = _project->gitRepo()->head().branchName();
    if (currentBranch != branch) {
        _tempRemote = remote;
        _tempBranch = branch;
        Utility::dialog(tr("Continue"), tr("Cancel"), tr("Confirm Push"),
            tr("Upstream branch does not have the same name as the current branch."),
            this, SLOT(onPushDialogFinished(bb::system::SystemUiResult::Type)));
    } else {
        push(remote, branch);
    }
}

void GitRepoPage::onPushDialogFinished(bb::system::SystemUiResult::Type type)
{
    if (_tempRemote && type == bb::system::SystemUiResult::ConfirmButtonSelection)
        push(_tempRemote, _tempBranch);
    _tempRemote = NULL;
    _tempBranch.clear();
}

void GitRepoPage::createRemote(const QString &name, const QString &url)
{
    emit workerCreateRemote(name, url, Helium::instance()->git()->sshCredentials());
}

void GitRepoPage::setRemoteUrl(LibQGit2::Remote *remote, const QString &url)
{
    remote->setUrl(url);
    remote->save();
    if (_branchPage)
        _branchPage->reload();
}

void GitRepoPage::clone(const QString &url)
{
    emit workerClone(url, _project->path(), Helium::instance()->git()->sshCredentials());
}

void GitRepoPage::pushRemoteInfoPage(GitRemoteInfoPage::Mode mode, LibQGit2::Remote *remote)
{
    if (!_remoteInfoPage) {
        _remoteInfoPage = new GitRemoteInfoPage(this);
        conn(this, SIGNAL(translatorChanged()),
            _remoteInfoPage, SLOT(onTranslatorChanged()));
    }
    _remoteInfoPage->setMode(mode, remote);
    parent()->push(_remoteInfoPage);
}

void GitRepoPage::pushCommitPage(const QString &hintMessage)
{
    if (!_commitPage) {
        _commitPage = new GitCommitPage(this);
        conn(this, SIGNAL(translatorChanged()),
            _commitPage, SLOT(onTranslatorChanged()));
    }
    _commitPage->setHintMessage(hintMessage.isNull() ?
            tr("Commit message") : hintMessage);
    parent()->push(_commitPage);
    _commitPage->focus();
}

void GitRepoPage::pushBranchPage()
{
    if (!_branchPage) {
        _branchPage = new GitBranchPage(this);
        conn(this, SIGNAL(translatorChanged()),
            _branchPage, SLOT(onTranslatorChanged()));
    }
    _branchPage->connectToRepoPage();
    parent()->push(_branchPage);
}

void GitRepoPage::pushLogPage(const LibQGit2::Reference &ref, LibQGit2::Remote *remote, GitLogPage::Actions actions)
{
    if (!_logPage) {
        _logPage = new GitLogPage(this);
        conn(this, SIGNAL(translatorChanged()),
            _logPage, SLOT(onTranslatorChanged()));
    }
    _logPage->setReference(ref, remote);
    _logPage->setActions(actions);
    parent()->push(_logPage);
}

void GitRepoPage::pushLogPage()
{
    pushLogPage(_project->gitRepo()->head());
}

void GitRepoPage::addSelections()
{
    const QVariantList &selections =  _statusListView->selectionList();
    QList<QString> paths;
    for (int i = 0; i < selections.size(); i++) {
        paths.append(_statusDataModel.data(selections[i].toList())
                .value<StatusDiffDelta>()
                .delta.newFile().path());
    }
    addPaths(paths);
}

// add progress bar
void GitRepoPage::addPaths(const QList<QString> &paths)
{
    emit workerAddPaths(paths);
}

void GitRepoPage::addAll()
{
    QList<QString> paths;
    for (size_t i = 0, size = _statusDataModel.statusList().entryCount(); i < size; i++) {
        const LibQGit2::DiffDelta &delta = _statusDataModel.statusList().entryByIndex(i).indexToWorkdir();
        if (validDiffDelta(delta))
            paths << delta.newFile().path();
    }
    addPaths(paths);
}

void GitRepoPage::resetSelections()
{
    const QVariantList &selections =  _statusListView->selectionList();
    QList<QString> paths;
    for (int i = 0; i < selections.size(); i++) {
        paths.append(_statusDataModel.data(selections[i].toList())
                .value<StatusDiffDelta>()
                .delta.newFile().path());
    }
    resetPaths(paths);
}

void GitRepoPage::resetMixed()
{
    emit workerReset(LibQGit2::Repository::Mixed);
}

void GitRepoPage::safeResetHard()
{
    Utility::dialog(tr("Continue"), tr("Cancel"), tr("Confirm Reset"),
            tr("Hard reset will wipe the changes in your working directory."),
            this, SLOT(onResetHardDialogFinished(bb::system::SystemUiResult::Type)));
}

void GitRepoPage::onResetHardDialogFinished(bb::system::SystemUiResult::Type type)
{
    if (type == bb::system::SystemUiResult::ConfirmButtonSelection)
        emit workerReset(LibQGit2::Repository::Hard);
}

void GitRepoPage::resetPaths(const QList<QString> &paths)
{
    emit workerResetPaths(paths);
}

void GitRepoPage::pushSettingsPage()
{
    if (!_settingsPage) {
        _settingsPage = new GitSettingsPage(Helium::instance()->git());
        conn(this, SIGNAL(translatorChanged()),
            _settingsPage, SLOT(onTranslatorChanged()));
    }
    parent()->push(_settingsPage);
}

void GitRepoPage::pushDiffPage(const LibQGit2::Patch &patch, GitDiffPage::Actions actions)
{
    if (!_diffPage) {
        _diffPage = new GitDiffPage(this);
        conn(this, SIGNAL(translatorChanged()),
            _diffPage, SLOT(onTranslatorChanged()));
    }
    _diffPage->setPatch(patch);
    _diffPage->setActions(actions);
    parent()->push(_diffPage);
}

void GitRepoPage::showDiffSelection()
{
    showDiffIndexPath(_statusListView->selected());
}

void GitRepoPage::showDiffIndexPath(const QVariantList &indexPath)
{
    if (indexPath.size() < 2)
        return;
    qDebug() << "DIFFING SELECTION" << indexPath;
    const StatusDiffDelta &sdelta = _statusDataModel.data(indexPath)
            .value<StatusDiffDelta>();
    LibQGit2::DiffOptions opts;
    opts.setPaths(QList<QString>() << sdelta.delta.newFile().path());
    LibQGit2::Diff diff;
    if (sdelta.delta.type() != LibQGit2::DiffDelta::Deleted) {
        switch (sdelta.type) {
            case HeadToIndex: {
                LibQGit2::Tree tree;
                if (!_project->gitRepo()->isHeadUnborn()) {
                    tree = _project->gitRepo()->head().peelToTree();
                }
                diff = _project->gitRepo()->diffTreeToIndex(tree, _project->gitRepo()->index(), opts);
                break;
            }
            case IndexToWorkdir:
                diff = _project->gitRepo()->diffIndexToWorkdir(_project->gitRepo()->index(), opts);
                break;
        }
    }
    if (diff.numDeltas() == 0) {
        Utility::toast(tr("No diff available"));
        return;
    }
    const LibQGit2::Patch &p = diff.patch(0);
    if (p.numHunks() == 0) {
        Utility::toast(tr("No hunk details available"));
        return;
    }
    switch (sdelta.type) {
        case HeadToIndex:
            pushDiffPage(p, GitDiffPage::Reset);
            break;
        case IndexToWorkdir:
            pushDiffPage(p, GitDiffPage::Add);
            break;
    }
}

void GitRepoPage::pushCommitInfoPage(const LibQGit2::Commit &commit, GitCommitInfoPage::Actions actions)
{
    if (!_commitInfoPage) {
        _commitInfoPage = new GitCommitInfoPage(this);
        conn(this, SIGNAL(translatorChanged()),
            _commitInfoPage, SLOT(onTranslatorChanged()));
    }
    _commitInfoPage->setCommit(commit);
    _commitInfoPage->setActions(actions);
    parent()->push(_commitInfoPage);
}

void GitRepoPage::selectAllOnIndex()
{
    selectAllChildren(QVariantList() << QVariant(0));
}

void GitRepoPage::selectAllOnWorkdir()
{
    selectAllChildren(QVariantList() << QVariant(1));
}

void GitRepoPage::selectAllChildren(const QVariantList &index)
{
    // prevent firing of selectionChangeStarted
    SignalBlocker blocker(_statusListView);
    int i = 0, size = _statusDataModel.childCount(index);
    for (; i < size; i++) {
        QVariantList ip = index;
        ip << QVariant(i);
        StatusDiffDelta sdelta = _statusDataModel.data(ip)
                .value<StatusDiffDelta>();
        switch (sdelta.delta.type()) {
            case LibQGit2::DiffDelta::Unknown:
            case LibQGit2::DiffDelta::Unmodified:
                break;
            default:
                _statusListView->select(ip, true);
        }
    }
    if (i > 0)
        reloadMultiSelectActionsEnabled();
}

void GitRepoPage::onTranslatorChanged(bool reload)
{
    PushablePage::onTranslatorChanged();
    // no repo
    _initAction->setTitle(tr("Init"));
    _cloneAction->setTitle(tr("Clone"));
    _reloadAction->setTitle(tr("Reload"));
    _settingsAction->setTitle(tr("Settings"));
    // repo
    _commitAction->setTitle(tr("Commit"));
    _branchesAction->setTitle(tr("Branches"));
    _logAction->setTitle(tr("Log"));
    _addAllAction->setTitle(tr("Add All"));
    _resetMixedAction->setTitle(tr("Reset (Mixed)"));
    _resetHardAction->setTitle(tr("Reset (Hard)"));
    _multiAddAction->setTitle(tr("Add"));
    _multiResetAction->setTitle(tr("Reset"));
    // rebase
    _rebaseNextAction->setTitle(tr("Next"));
    _rebaseAbortAction->setTitle(tr("Abort"));
    // merge
    _mergeAbortAction->setTitle(tr("Abort"));
    if (reload)
        this->reload();
    emit translatorChanged();
}

void GitRepoPage::StatusDataModel::setStatusList(const LibQGit2::StatusList &list)
{
    DataModelChangeType::Type changeType = DataModelChangeType::Init;
    // if == 0 then do an Init, as there might still be elements left over
    // after resetStatusList
    if (_statusList.entryCount() != 0 && list.entryCount() == _statusList.entryCount())
        changeType = DataModelChangeType::Update;
    _statusList = list;
    emit itemsChanged(changeType);
}

void GitRepoPage::StatusDataModel::resetStatusList()
{
    _statusList = LibQGit2::StatusList();
}

bool GitRepoPage::StatusDataModel::hasValidDiffDeltasInWorkdir() const
{
    for (size_t i = 0, size = _statusList.entryCount(); i < size; i++) {
        if (validDiffDelta(_statusList.entryByIndex(i).indexToWorkdir()))
            return true;
    }
    return false;
}

bool GitRepoPage::StatusDataModel::hasValidDiffDeltasInIndex() const
{
    for (size_t i = 0, size = _statusList.entryCount(); i < size; i++) {
        if (validDiffDelta(_statusList.entryByIndex(i).headToIndex()))
            return true;
    }
    return false;
}

int GitRepoPage::StatusDataModel::childCount(const QVariantList &indexPath)
{
    if (indexPath.empty())
        return 2;
    if (indexPath.size() == 1) {
        switch (indexPath[0].toInt()) {
            case 0: case 1:
                return _statusList.entryCount();
        }
    }
    return 0;
}

bool GitRepoPage::StatusDataModel::hasChildren(const QVariantList &indexPath)
{
    if (indexPath.empty())
        return true;
    if (indexPath.size() == 1)  {
        switch (indexPath[0].toInt()) {
            case 0: case 1:
                return true;
        }
    }
    return false;
}

QString GitRepoPage::StatusDataModel::itemType(const QVariantList &indexPath)
{
    if (indexPath.size() < 2)
        return "header";
    switch (indexPath[0].toInt()) {
        case 0:
            return "headToIndexItem";
        case 1:
            return "indexToWorkdirItem";
    }
    return QString();
}

QVariant GitRepoPage::StatusDataModel::data(const QVariantList &indexPath)
{
    if (!indexPath.empty()) {
        switch (indexPath[0].toInt()) {
            case 0:
                if (indexPath.size() < 2)
                    return tr("Index");
                return QVariant::fromValue(StatusDiffDelta(
                        HeadToIndex,
                        _statusList.entryByIndex(indexPath[1].toInt()).headToIndex()));
            case 1:
                if (indexPath.size() < 2)
                    return tr("Working Directory");
                return QVariant::fromValue(StatusDiffDelta(
                        IndexToWorkdir,
                        _statusList.entryByIndex(indexPath[1].toInt()).indexToWorkdir()));
        }
    }
    return QVariant();
}

VisualNode *GitRepoPage::StatusItemProvider::createItem(ListView *, const QString &type)
{
    if (type == "header")
        return Header::create();
    if (type == "headToIndexItem")
        return StandardListItem::create()
            .actionSet(ActionSet::create()
                    .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "View Diff"))
                        .reloadTitleOn(_gitRepoPage, SIGNAL(translatorChanged()))
                        .imageSource(QUrl("asset:///images/ic_git_diff.png"))
                        .onTriggered(_gitRepoPage, SLOT(showDiffSelection())))
                    .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Reset"))
                        .reloadTitleOn(_gitRepoPage, SIGNAL(translatorChanged()))
                        .imageSource(QUrl("asset:///images/ic_remove.png"))
                        .onTriggered(_gitRepoPage, SLOT(resetSelections())))
                    .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Select All"))
                        .reloadTitleOn(_gitRepoPage, SIGNAL(translatorChanged()))
                        .imageSource(QUrl("asset:///images/ic_select_all.png"))
                        .onTriggered(_gitRepoPage, SLOT(onSelectAllOnIndexTriggered()))));
    if (type == "indexToWorkdirItem")
        return StandardListItem::create()
            .actionSet(ActionSet::create()
                    .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "View Diff"))
                        .reloadTitleOn(_gitRepoPage, SIGNAL(translatorChanged()))
                        .imageSource(QUrl("asset:///images/ic_git_diff.png"))
                        .onTriggered(_gitRepoPage, SLOT(showDiffSelection())))
                    .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Add"))
                        .reloadTitleOn(_gitRepoPage, SIGNAL(translatorChanged()))
                        .imageSource(QUrl("asset:///images/ic_add.png"))
                        .onTriggered(_gitRepoPage, SLOT(addSelections())))
                    .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Select All"))
                        .reloadTitleOn(_gitRepoPage, SIGNAL(translatorChanged()))
                        .imageSource(QUrl("asset:///images/ic_select_all.png"))
                        .onTriggered(_gitRepoPage, SLOT(onSelectAllOnWorkdirTriggered()))));
    return NULL;
}

void GitRepoPage::StatusItemProvider::updateItem(ListView *, VisualNode *listItem, const QString &type,
            const QVariantList &, const QVariant &data)
{
    if (type == "header") {
        ((Header *) listItem)->setTitle(data.toString());
    } else if (type == "headToIndexItem" || type == "indexToWorkdirItem") {
        StandardListItem *li = (StandardListItem *) listItem;
        const StatusDiffDelta &sdelta = data.value<StatusDiffDelta>();
        li->setVisible(true);
        switch (sdelta.delta.type()) {
            case LibQGit2::DiffDelta::Unknown:
                li->setVisible(false);
                break;
            case LibQGit2::DiffDelta::Unmodified:
                li->setVisible(false);
                break;
            case LibQGit2::DiffDelta::Added:
                li->setTitle(sdelta.delta.newFile().path());
                li->setStatus(tr("Added"));
                break;
            case LibQGit2::DiffDelta::Deleted:
                li->setTitle(sdelta.delta.oldFile().path());
                li->setStatus(tr("Deleted"));
                break;
            case LibQGit2::DiffDelta::Modified:
                li->setTitle(sdelta.delta.newFile().path());
                li->setStatus(tr("Modified"));
                break;
            case LibQGit2::DiffDelta::Renamed:
                li->setTitle(sdelta.delta.newFile().path());
                li->setDescription(tr("was %1").arg(sdelta.delta.oldFile().path()));
                li->setStatus(tr("Renamed"));
                break;
            case LibQGit2::DiffDelta::Copied:
                li->setTitle(sdelta.delta.newFile().path());
                li->setStatus(tr("Copied"));
                break;
            case LibQGit2::DiffDelta::Ignored:
                li->setTitle(sdelta.delta.newFile().path());
                li->setStatus(tr("Ignored"));
                break;
            case LibQGit2::DiffDelta::Untracked:
                li->setTitle(sdelta.delta.newFile().path());
                li->setStatus(tr("Untracked"));
                break;
            case LibQGit2::DiffDelta::Typechange:
                li->setTitle(sdelta.delta.newFile().path());
                li->setStatus(tr("Type Changed"));
                break;
        }
    }
}

void GitRepoPage::onPagePopped(Page *page)
{
    if (!page)
        return;
    if (page == this) {
        _statusDataModel.resetStatusList();
        resetProject();
    } else if (page == _diffPage)
        _diffPage->resetPatch();
    else if (page == _commitInfoPage)
        _commitInfoPage->resetCommit();
    else if (page == _remoteInfoPage)
        _remoteInfoPage->resetRemote();
    else if (page == _branchPage) {
        _branchPage->reset();
        _branchPage->disconnectFromRepoPage();
    }
}

void GitRepoPage::onGitWorkerInProgressChanged(bool inProgress)
{
    if (inProgress) {
        // lock the content
        _statusListView->setEnabled(false);
        for (int i = 0; i < actionCount(); i++)
            actionAt(i)->setEnabled(false);
    } else
        reloadContent();
}

void GitRepoPage::onProjectPathChanged()
{
    if (parent()->indexOf(this) >= 0) {
        // pop any page
        parent()->navigateTo(this);
        reload();
    }
}

void GitRepoPage::onSelectAllOnIndexTriggered()
{
    _statusListView->multiSelectHandler()->setActive(true);
    QTimer::singleShot(0, this, SLOT(selectAllOnIndex()));
}

void GitRepoPage::onSelectAllOnWorkdirTriggered()
{
    _statusListView->multiSelectHandler()->setActive(true);
    QTimer::singleShot(0, this, SLOT(selectAllOnWorkdir()));
}
