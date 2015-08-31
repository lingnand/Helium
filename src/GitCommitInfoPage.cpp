/*
 * GitCommitInfoPage.cpp
 *
 *  Created on: Aug 24, 2015
 *      Author: lingnan
 */

#include <bb/cascades/TitleBar>
#include <bb/cascades/Header>
#include <bb/cascades/Label>
#include <bb/cascades/StandardListItem>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/ListView>
#include <bb/cascades/ActionSet>
#include <bb/cascades/Shortcut>
#include <libqgit2/qgitrepository.h>
#include <libqgit2/qgitdifffile.h>
#include <libqgit2/qgitpatch.h>
#include <GitCommitInfoPage.h>
#include <GitRepoPage.h>
#include <GitDiffPage.h>
#include <Segment.h>
#include <Defaults.h>
#include <Utility.h>
#include <LocaleAwareActionItem.h>

using namespace bb::cascades;

GitCommitInfoPage::GitCommitInfoPage(GitRepoPage *page):
    _repoPage(page),
    _itemProvider(this),
    _listView(ListView::create()
        .dataModel(&_dataModel)
        .listItemProvider(&_itemProvider)),
    _checkoutAction(NULL)
{
    conn(_listView, SIGNAL(triggered(QVariantList)),
        this, SLOT(showDiffIndexPath(const QVariantList &)));

    setTitleBar(TitleBar::create());

    setContent(_listView);

    onTranslatorChanged();
}


void GitCommitInfoPage::setCommit(const LibQGit2::Commit &commit)
{
    QList<Diff> diffs;
    const LibQGit2::Tree &t = commit.tree();
    unsigned int i = 0;
    for (unsigned int count = commit.parentCount(); i < count; i++) {
        const LibQGit2::Commit &parent = commit.parent(i);
        diffs.append(Diff(_repoPage->repo()->diffTrees(parent.tree(), t),
                QString(parent.oid().nformat(7))));
    }
    if (i == 0) {
        diffs.append(Diff(_repoPage->repo()->diffTrees(LibQGit2::Tree(), t)));
    }
    _dataModel.setCommit(commit, diffs);
}

void GitCommitInfoPage::resetCommit()
{
    _dataModel.clear();
}

void GitCommitInfoPage::showDiffSelection()
{
    showDiffIndexPath(_listView->selected());
}

void GitCommitInfoPage::showDiffIndexPath(const QVariantList &ip)
{
    if (ip.size() < 2)
        return;
    int index = ip[0].toInt();
    if (index < 3)
        return;
    const LibQGit2::Patch &p = _dataModel.diffs()[index-3].diff.patch(ip[1].toInt());
    if (p.numHunks() == 0) {
        Utility::toast(tr("No hunk details available"));
        return;
    }
    _repoPage->pushDiffPage(p);
}

void GitCommitInfoPage::checkout()
{
    if (_repoPage->checkout(_dataModel.commit()))
        pop();
}

void GitCommitInfoPage::setActions(Actions actions)
{
    while (actionCount() > 0)
        removeAction(actionAt(0));
    if (actions.testFlag(Checkout)) {
        if (!_checkoutAction)
            _checkoutAction = LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "Checkout"))
                .reloadTitleOn(this, SIGNAL(translatorChanged()))
                .addShortcut(Shortcut::create().key("c"))
                .onTriggered(this, SLOT(checkout()));
        addAction(_checkoutAction, ActionBarPlacement::Signature);
    }
}

void GitCommitInfoPage::onTranslatorChanged()
{
    PushablePage::onTranslatorChanged();
    titleBar()->setTitle(tr("Commit Details"));
    _dataModel.refresh();
    emit translatorChanged();
}


int GitCommitInfoPage::DiffDataModel::childCount(const QVariantList &ip)
{
    switch (ip.size()) {
        case 0:
            return 3 + _diffs.size();
        case 1: {
            int index = ip[0].toInt();
            if (index < 3)
                return 1;
            return _diffs[index-3].diff.numDeltas();
        }
    }
    return 0;
}

bool GitCommitInfoPage::DiffDataModel::hasChildren(const QVariantList &ip)
{
    return ip.size() < 2;
}

QString GitCommitInfoPage::DiffDataModel::itemType(const QVariantList &ip)
{
    switch (ip.size()) {
        case 1:
            return "header";
        case 2:
            switch (ip[0].toInt()) {
                case 0: case 1:
                    return "sigitem";
                case 2:
                    return "message";
                default:
                    return "diffitem";
            }
    }
    return QString();
}

QVariant GitCommitInfoPage::DiffDataModel::data(const QVariantList &ip)
{
    if (ip.empty())
        return QVariant();
    int index = ip[0].toInt();
    switch (index) {
        case 0:
            if (ip.size() < 2)
                return tr("Author");
            return QVariant::fromValue(_commit.author());
        case 1:
            if (ip.size() < 2)
                return tr("Committer");
            return QVariant::fromValue(_commit.committer());
        case 2:
            if (ip.size() < 2)
                return tr("Message");
            return _commit.message();
        default:
            const Diff diff = _diffs[index-3];
            if (ip.size() < 2) {
                if (diff.parentOId.isEmpty())
                    return tr("Diff");
                return tr("Diff with Parent %1").arg(diff.parentOId);
            }
            return QVariant::fromValue(diff.diff.delta(ip[1].toInt()));
    }
}

void GitCommitInfoPage::DiffDataModel::setCommit(const LibQGit2::Commit &commit, const QList<Diff> &diffs)
{
    _commit = commit;
    // populating the diffs
    _diffs = diffs;
    emit itemsChanged(DataModelChangeType::Init);
}

void GitCommitInfoPage::DiffDataModel::clear()
{
    _diffs.clear();
    _commit = LibQGit2::Commit();
}

void GitCommitInfoPage::DiffDataModel::refresh()
{
    emit itemsChanged(DataModelChangeType::Init);
}

GitCommitInfoPage::LabelSegment::LabelSegment():
    label(Label::create().multiline(true)
        .textStyle(SystemDefaults::TextStyles::primaryText()))
{
    setLeftPadding(Defaults::space());
    setRightPadding(Defaults::space());
    setTopPadding(Defaults::space());
    setBottomPadding(Defaults::space());
    add(label);
}

VisualNode *GitCommitInfoPage::DiffItemProvider::createItem(ListView *, const QString &type)
{
    if (type == "header")
        return Header::create();
    if (type == "message")
        return new LabelSegment;
    if (type == "sigitem")
        return StandardListItem::create();
    if (type == "diffitem")
        return StandardListItem::create()
                .actionSet(ActionSet::create()
                    .add(LocaleAwareActionItem::create(QT_TRANSLATE_NOOP("Man", "View Diff"))
                        .reloadTitleOn(_page, SIGNAL(translatorChanged()))
                        .onTriggered(_page, SLOT(showDiffSelection()))));
    return NULL;
}

void GitCommitInfoPage::DiffItemProvider::updateItem(ListView *, VisualNode *item, const QString &type,
    const QVariantList &, const QVariant &data)
{
    if (type == "header")
        ((Header *) item)->setTitle(data.toString());
    else if (type == "message")
        ((LabelSegment *) item)->label->setText(data.toString());
    else if (type == "sigitem") {
        StandardListItem *li = (StandardListItem *) item;
        const LibQGit2::Signature &sig = data.value<LibQGit2::Signature>();
        li->setTitle(sig.name());
        li->setDescription(sig.email());
        li->setStatus(sig.when().toString());
    } else if (type == "diffitem") {
        StandardListItem *li = (StandardListItem *) item;
        const LibQGit2::DiffDelta &delta = data.value<LibQGit2::DiffDelta>();
        li->setVisible(true);
        switch (delta.type()) {
            case LibQGit2::DiffDelta::Unknown:
                li->setVisible(false);
                break;
            case LibQGit2::DiffDelta::Unmodified:
                li->setVisible(false);
                break;
            case LibQGit2::DiffDelta::Added:
                li->setTitle(delta.newFile().path());
                li->setStatus(tr("Added"));
                break;
            case LibQGit2::DiffDelta::Deleted:
                li->setTitle(delta.oldFile().path());
                li->setStatus(tr("Deleted"));
                break;
            case LibQGit2::DiffDelta::Modified:
                li->setTitle(delta.newFile().path());
                li->setStatus(tr("Modified"));
                break;
            case LibQGit2::DiffDelta::Renamed:
                li->setTitle(delta.newFile().path());
                li->setDescription(tr("was %1").arg(delta.oldFile().path()));
                li->setStatus(tr("Renamed"));
                break;
            case LibQGit2::DiffDelta::Copied:
                li->setTitle(delta.newFile().path());
                li->setStatus(tr("Copied"));
                break;
            case LibQGit2::DiffDelta::Ignored:
                li->setTitle(delta.newFile().path());
                li->setStatus(tr("Ignored"));
                break;
            case LibQGit2::DiffDelta::Untracked:
                li->setTitle(delta.newFile().path());
                li->setStatus(tr("Untracked"));
                break;
            case LibQGit2::DiffDelta::Typechange:
                li->setTitle(delta.newFile().path());
                li->setStatus(tr("Type Changed"));
                break;
        }
    }
}
