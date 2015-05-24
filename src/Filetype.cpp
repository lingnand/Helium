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
        RunProfileManager *runProfileManager,
        QObject *parent):
    QObject(parent),
    _name(name),
    _highlightEnabled(highlightEnabled),
    _runProfileManager(NULL)
{
    setRunProfileManager(runProfileManager);
}

std::string Filetype::langName() const
{
    return std::string(_name.toUtf8().constData()) + ".lang";
}

void Filetype::setHighlightEnabled(bool enabled)
{
    if (enabled != _highlightEnabled) {
        _highlightEnabled = enabled;
        emit highlightEnabledChanged(_highlightEnabled);
        emit highlightTypeChanged(highlightType());
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
