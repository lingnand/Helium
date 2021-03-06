/*
 * Filetype.cpp
 *
 *  Created on: May 15, 2015
 *      Author: lingnan
 */

#include <QDebug>
#include <Filetype.h>
#include <RunProfileManager.h>

Filetype::Filetype(const QString &name,
        bool highlightEnabled,
        bool tabSpaceConversionEnabled,
        int numberOfSpacesForTab,
        RunProfileManager *runProfileManager,
        QObject *parent):
    QObject(parent),
    _name(name),
    _highlightEnabled(highlightEnabled),
    _tabSpaceConversionEnabled(tabSpaceConversionEnabled),
    _numberOfSpacesForTab(numberOfSpacesForTab),
    _runProfileManager(NULL)
{
    setRunProfileManager(runProfileManager);
}

void Filetype::setHighlightEnabled(bool enabled)
{
    if (enabled != _highlightEnabled) {
        _highlightEnabled = enabled;
        emit highlightEnabledChanged(_highlightEnabled);
    }
}

void Filetype::setTabSpaceConversionEnabled(bool enabled)
{
    if (enabled != _tabSpaceConversionEnabled) {
        _tabSpaceConversionEnabled = enabled;
        emit tabSpaceConversionEnabledChanged(_tabSpaceConversionEnabled);
    }
}

void Filetype::setNumberOfSpacesForTab(int number)
{
    if (number != _numberOfSpacesForTab) {
        _numberOfSpacesForTab = number;
        emit numberOfSpacesForTabChanged(_numberOfSpacesForTab);
    }
}

void Filetype::setRunProfileManager(RunProfileManager *runProfileManager)
{
    if (runProfileManager != _runProfileManager) {
        Q_ASSERT(!runProfileManager || !runProfileManager->parent());
        RunProfileManager *old = _runProfileManager;
        if (old)
            old->deleteLater();
        _runProfileManager = runProfileManager;
        if (_runProfileManager)
            _runProfileManager->setParent(this);
        emit runProfileManagerChanged(_runProfileManager, old);
    }
}

QDebug operator<<(QDebug dbg, const Filetype *filetype)
{
    if (filetype) {
        dbg.nospace() << "Filetype(" << filetype->name()
                << ", highlightEnabled:" << filetype->highlightEnabled() << ")";
    } else {
        dbg << "Filetype(NULL)";
    }
    return dbg.space();
}
