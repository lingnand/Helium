/*
 * Filetype.cpp
 *
 *  Created on: May 15, 2015
 *      Author: lingnan
 */

#include <QDebug>
#include <Filetype.h>

Filetype::Filetype(const QString &name,
        bool highlightEnabled,
        RunProfileManager *runProfileManager,
        QObject *parent):
    QObject(parent),
    _name(name),
    _highlightEnabled(highlightEnabled),
    _runProfileManager(runProfileManager)
{
}

const QString &Filetype::name() const
{
    return _name;
}

std::string Filetype::langName() const
{
    return std::string(_name.toUtf8().constData()) + ".lang";
}

RunProfileManager *Filetype::runProfileManager() const
{
    return _runProfileManager;
}

bool Filetype::highlightEnabled() const
{
    return _highlightEnabled;
}

void Filetype::setHighlightEnabled(bool enabled)
{
    if (enabled != _highlightEnabled) {
        _highlightEnabled = enabled;
        emit highlightEnabledChanged(_highlightEnabled);
    }
}

void Filetype::setRunProfileManager(RunProfileManager *runProfileManager)
{
    if (runProfileManager != _runProfileManager) {
        _runProfileManager = runProfileManager;
        emit runProfileManagerChanged(_runProfileManager);
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
