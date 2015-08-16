/*
 * GitRepoPage.cpp
 *
 *  Created on: Aug 8, 2015
 *      Author: lingnan
 */

#include <bb/cascades/TitleBar>
#include <bb/cascades/MultiSelectActionItem>
#include <bb/cascades/ActionItem>
#include <bb/cascades/Shortcut>
#include <bb/cascades/ScrollView>
#include <bb/cascades/ListView>
#include <bb/cascades/Label>
#include <bb/cascades/Header>
#include <bb/cascades/StandardListItem>
#include <bb/cascades/NavigationPane>
#include <libqgit2/qgitrepository.h>
#include <libqgit2/qgitexception.h>
#include <libqgit2/qgitdifffile.h>
#include <libqgit2/qgitdiffdelta.h>
#include <libqgit2/qgitdiff.h>
#include <Helium.h>
#include <GitSettings.h>
#include <Defaults.h>
#include <GitRepoPage.h>
#include <GitDiffPage.h>
#include <GitCommitPage.h>
#include <StatusActionSet.h>
#include <Project.h>
#include <Segment.h>
#include <Utility.h>

using namespace bb::cascades;

GitRepoPage::GitRepoPage(Project *project):
    _project(project),
    _initAction(ActionItem::create()
        .addShortcut(Shortcut::create().key("i"))
        .onTriggered(this, SLOT(init()))),
    _cloneAction(ActionItem::create()
        .addShortcut(Shortcut::create().key("c"))
        .onTriggered(this, SLOT(clone()))),
    _reloadAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_reload.png"))
        .addShortcut(Shortcut::create().key("l"))
        .onTriggered(this, SLOT(reload()))),
    _noRepoLabel(Label::create().multiline(true)
        .format(TextFormat::Html)
        .preferredWidth(0)
        .contentFlags(TextContentFlag::ActiveTextOff)
        .textStyle(Defaults::centeredBodyText())),
    _noRepoContent(ScrollView::create(
        Segment::create().section().subsection()
            .add(_noRepoLabel))),
    _commitAction(ActionItem::create()
        .addShortcut(Shortcut::create().key("c"))
        .onTriggered(this, SLOT(showCommitPage()))),
    _branchesAction(ActionItem::create()
        .addShortcut(Shortcut::create().key("b"))
        .onTriggered(this, SLOT(branches()))),
    _logAction(ActionItem::create()
        .addShortcut(Shortcut::create().key("l"))
        .onTriggered(this, SLOT(log()))),
    _statusItemProvider(this),
    _repoContent(ListView::create()
        .dataModel(&_statusDataModel)
        .listItemProvider(&_statusItemProvider)),
    _multiAddAction(ActionItem::create()
        .onTriggered(this, SLOT(addSelections()))),
    _multiResetAction(ActionItem::create()
        .onTriggered(this, SLOT(resetSelections()))),
    _diffPage(NULL),
    _commitPage(NULL)
{
    _repoContent->setMultiSelectAction(MultiSelectActionItem::create());
    _repoContent->multiSelectHandler()->addAction(_multiAddAction);
    _repoContent->multiSelectHandler()->addAction(_multiResetAction);
    conn(_repoContent, SIGNAL(triggered(QVariantList)),
        this, SLOT(showDiffIndexPath(const QVariantList &)));
    setTitleBar(TitleBar::create());

    conn(_project, SIGNAL(pathChanged(const QString&)),
        this, SLOT(onProjectPathChanged()));

    onTranslatorChanged();
}

void GitRepoPage::hideAllActions()
{
    while (actionCount() > 0)
        removeAction(actionAt(0));
}

void GitRepoPage::reload()
{
    try {
        // title
        if (_project->gitRepo()->isHeadUnborn())
            titleBar()->setTitle(tr("No commit"));
        else
            titleBar()->setTitle(_project->gitRepo()->head().name());
        // actions
        hideAllActions();
        addAction(_commitAction, ActionBarPlacement::Signature);
        addAction(_branchesAction, ActionBarPlacement::OnBar);
        addAction(_logAction, ActionBarPlacement::OnBar);
        addAction(_reloadAction);
        // content
        const LibQGit2::StatusList &list = _project->gitRepo()->status(
                LibQGit2::StatusOptions(LibQGit2::StatusOptions::ShowIndexAndWorkdir,
                        LibQGit2::StatusOptions::IncludeUntracked |
//                        LibQGit2::StatusOptions::ExcludeSubmodules |
                        LibQGit2::StatusOptions::RenamesHeadToIndex |
                        LibQGit2::StatusOptions::RenamesIndexToWorkdir));
        _statusDataModel.setStatusList(list);
        _commitAction->setEnabled(_project->gitRepo()->index().entryCount() > 0);
        setContent(_repoContent);
    } catch (const LibQGit2::Exception &e) {
        qDebug() << "::::LIBQGIT2 ERROR when reloading RepoPage::::" << e.what();
        // title
        titleBar()->setTitle(tr("No Repository"));
        // actions
        hideAllActions();
        addAction(_initAction, ActionBarPlacement::Signature);
        addAction(_cloneAction, ActionBarPlacement::OnBar);
        addAction(_reloadAction, ActionBarPlacement::OnBar);
        // content
        setContent(_noRepoContent);
    }
}

void GitRepoPage::init()
{
    _project->gitRepo()->init(_project->path());
    reload();
}

void GitRepoPage::clone()
{

}

bool GitRepoPage::commit(const QString &message)
{
    try {
        LibQGit2::Repository *repo = _project->gitRepo();
        // pop open a new page to record the commit message
        QList<LibQGit2::Commit> parents;
        if (!repo->isHeadUnborn()) {
            parents.append(repo->lookupCommit(repo->head().resolve().target()));
        }
        // obtain the signature directly from global config
        GitSettings *git = Helium::instance()->git();
        LibQGit2::Signature sig(git->name(), git->email());
        LibQGit2::OId oid = repo->createCommit(repo->lookupTree(repo->index().createTree()),
                parents, sig, sig, message, "HEAD");
        Utility::toast(tr("Commited %1").arg(QString(oid.nformat(7))));
        // rewrite index
        repo->index().write();
    } catch (const LibQGit2::Exception &e) {
        Utility::toast(e.what());
        return false;
    }
    reload();
    return true;
}

void GitRepoPage::showCommitPage()
{
    if (!_commitPage) {
        _commitPage = new GitCommitPage(this);
        conn(this, SIGNAL(translatorChanged()),
            _commitPage, SLOT(onTranslatorChanged()));
    }
    parent()->push(_commitPage);
    _commitPage->focus();
}

void GitRepoPage::branches()
{

}

void GitRepoPage::log()
{

}

void GitRepoPage::resetSelections()
{
    const QVariantList &selections =  _repoContent->selectionList();
    QList<QString> paths;
    for (int i = 0; i < selections.size(); i++) {
        paths.append(_statusDataModel.data(selections[i].toList())
                .value<StatusDiffDelta>()
                .delta.newFile().path());
    }
    resetPaths(paths);
}

void GitRepoPage::addSelections()
{
    const QVariantList &selections =  _repoContent->selectionList();
    QList<QString> paths;
    for (int i = 0; i < selections.size(); i++) {
        paths.append(_statusDataModel.data(selections[i].toList())
                .value<StatusDiffDelta>()
                .delta.newFile().path());
    }
    addPaths(paths);
}

void GitRepoPage::addPaths(const QList<QString> &paths)
{
    try {
        for (int i = 0; i < paths.size(); i++)
            _project->gitRepo()->index().addByPath(paths[i]);
    } catch (const LibQGit2::Exception &e) {
        Utility::toast(e.what());
    }
    reload();
}

void GitRepoPage::resetPaths(const QList<QString> &paths)
{
    try {
        for (int i = 0; i < paths.size(); i++)
            _project->gitRepo()->index().remove(paths[i], 0);
    } catch (const LibQGit2::Exception &e) {
        Utility::toast(e.what());
    }
    reload();
}

void GitRepoPage::showDiffSelection()
{
    showDiffIndexPath(_repoContent->selected());
}

void GitRepoPage::showDiffIndexPath(const QVariantList &indexPath)
{
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
                    tree = _project->gitRepo()->lookupTree(
                            _project->gitRepo()->head().resolve().target());
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
        Utility::toast("No diff available");
        return;
    }
    qDebug() << "NUM DIFF DELTAS" << diff.numDeltas();
    if (!_diffPage) {
        _diffPage = new GitDiffPage(this);
        conn(this, SIGNAL(translatorChanged()),
            _diffPage, SLOT(onTranslatorChanged()));
    }
    _diffPage->setPatch(StatusPatch(sdelta.type, diff.patch(0)));
    parent()->push(_diffPage);
}

void GitRepoPage::onTranslatorChanged()
{
    PushablePage::onTranslatorChanged();
    // no repo
    _initAction->setTitle(tr("Init"));
    _cloneAction->setTitle(tr("Clone"));
    _reloadAction->setTitle(tr("Reload"));
    _noRepoLabel->setText(tr("<br/>No repository found.<br/><br/>Use <b>Init</b> or <b>Clone</b> to create a git repository in<br/><em>%1</em>")
            .arg(_project->path()));
    _commitAction->setTitle(tr("Commit"));
    _branchesAction->setTitle(tr("Branches"));
    _logAction->setTitle(tr("Log"));
    _multiAddAction->setTitle(tr("Add"));
    _multiResetAction->setTitle(tr("Reset"));
    emit translatorChanged();
}

void GitRepoPage::StatusDataModel::setStatusList(const LibQGit2::StatusList &list)
{
    DataModelChangeType::Type changeType = DataModelChangeType::Init;
    // if == 0 then do an Init, as there might still be elements left over
    // after resetStatusList
    if (list.entryCount() != 0 && list.entryCount() == _statusList.entryCount())
        changeType = DataModelChangeType::Update;
    _statusList = list;
    emit itemsChanged(changeType);
}

void GitRepoPage::StatusDataModel::resetStatusList()
{
    _statusList = LibQGit2::StatusList();
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
    if (indexPath.size() == 1)
        return "header";
    return "item";
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

VisualNode *GitRepoPage::StatusItemProvider::createItem(ListView *list, const QString &type)
{
    if (type == "header")
        return Header::create();
    return StandardListItem::create();
}

void GitRepoPage::StatusItemProvider::updateItem(ListView *list, VisualNode *listItem, const QString &type,
            const QVariantList &indexPath, const QVariant &data)
{
    if (type == "header") {
        ((Header *) listItem)->setTitle(data.toString());
    } else if (type == "item" && indexPath.size() > 1) {
        StandardListItem *li = (StandardListItem *) listItem;
        StatusDiffDelta sdelta = data.value<StatusDiffDelta>();
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
        li->removeAllActionSets();
        li->addActionSet(new StatusActionSet(_gitRepoPage, sdelta));
    }
}

void GitRepoPage::onPagePopped(Page *page)
{
    if (page == this)
        _statusDataModel.resetStatusList();
    else if (page == _diffPage)
        _diffPage->resetPatch();
}

void GitRepoPage::onProjectPathChanged()
{
    if (parent()->indexOf(this) >= 0) {
        // pop any page
        parent()->navigateTo(this);
        reload();
    }
}
