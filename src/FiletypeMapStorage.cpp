/*
 * FiletypeMapStorage.cpp
 *
 *  Created on: May 17, 2015
 *      Author: lingnan
 */

#include <QStringList>
#include <QDebug>
#include <FiletypeMapStorage.h>
#include <Filetype.h>
#include <FiletypeMap.h>
#include <CmdRunProfileManager.h>
#include <WebRunProfileManager.h>
#include <Utility.h>

FiletypeMapStorage::FiletypeMapStorage(const QString &prefix, QObject *parent):
    QObject(parent)
{
    _settings.beginGroup(prefix);
}

void FiletypeMapStorage::onFiletypeRunProfileManagerChanged(RunProfileManager *change, RunProfileManager *old)
{
    if (old) {
        old->disconnect(this);
        _settings.remove(old->parent()->name() + "/run_profile_manager");
    }
    insertRunProfileManager(change);
}

void FiletypeMapStorage::insertRunProfileManager(RunProfileManager *manager)
{
    if (manager) {
        _settings.beginGroup(manager->parent()->name() + "/run_profile_manager");
        qDebug() << "Inserting RunProfileManager to Settings...";
        if (CmdRunProfileManager *cm = dynamic_cast<CmdRunProfileManager *>(manager)) {
            qDebug() << "CmdRunProfileManager detected...";
            _settings.setValue("type", RunProfileManager::Cmd);
            _settings.setValue("cmd", cm->cmd());
            connectCmdRunProfileManager(cm);
        } else if (WebRunProfileManager *wm = dynamic_cast<WebRunProfileManager *>(manager)) {
            qDebug() << "WebRunProfileManager detected...";
            _settings.setValue("type", RunProfileManager::Web);
            _settings.setValue("mode", wm->mode());
            connectWebRunProfileManager(wm);
        }
        _settings.endGroup();
    }
}

void FiletypeMapStorage::insertFiletype(Filetype *filetype)
{
    qDebug() << "inserting filetype" << filetype->name();
    _settings.setValue(filetype->name()+"/highlight_enabled", filetype->highlightEnabled());
    insertRunProfileManager(filetype->runProfileManager());
    connectFiletype(filetype);
}

void FiletypeMapStorage::connectFiletype(Filetype *filetype)
{
    conn(filetype, SIGNAL(highlightEnabledChanged(bool)),
            this, SLOT(onFiletypeHighlightEnabledChanged(bool)));
    conn(filetype, SIGNAL(runProfileManagerChanged(RunProfileManager*, RunProfileManager*)),
            this, SLOT(onFiletypeRunProfileManagerChanged(RunProfileManager*, RunProfileManager*)));
}

void FiletypeMapStorage::connectCmdRunProfileManager(CmdRunProfileManager *m)
{
    conn(m, SIGNAL(cmdChanged(const QString&)),
            this, SLOT(onCmdRunProfileManagerCmdChanged(const QString&)));
}

void FiletypeMapStorage::connectWebRunProfileManager(WebRunProfileManager *m)
{
    conn(m, SIGNAL(modeChanged(WebRunProfile::Mode)),
            this, SLOT(onWebRunProfileManagerModeChanged(WebRunProfile::Mode)));
}

void FiletypeMapStorage::onCmdRunProfileManagerCmdChanged(const QString &cmd)
{
    CmdRunProfileManager *m = (CmdRunProfileManager *) sender();
    qDebug() << "Saving CmdRunProfileManager cmd into" << m->parent()->name();
    _settings.setValue(m->parent()->name()+"/run_profile_manager/cmd", cmd);
}

void FiletypeMapStorage::onWebRunProfileManagerModeChanged(WebRunProfile::Mode mode)
{
    WebRunProfileManager *m = (WebRunProfileManager *) sender();
    qDebug() << "Saving WebRunProfileManager mode into" << m->parent()->name();
    _settings.setValue(m->parent()->name()+"/run_profile_manager/mode", mode);
}

void FiletypeMapStorage::onFiletypeHighlightEnabledChanged(bool enabled)
{
    Filetype *f = (Filetype *) sender();
    _settings.setValue(f->name()+"/highlight_enabled", enabled);
}

FiletypeMap *FiletypeMapStorage::read()
{
    FiletypeMap *map = new FiletypeMap(this);
    QStringList keys = _settings.childGroups();
    if (keys.empty()) {
        qDebug() << "Populating default filetypeMap...";
        *map << new Filetype("ada", true, NULL, this)
            << new Filetype("applescript", true, NULL, this)
            << new Filetype("asm", true, NULL, this)
            << new Filetype("awk", true, NULL, this)
            << new Filetype("bat", true, NULL, this)
            << new Filetype("bib", true, NULL, this)
            << new Filetype("bison", true, NULL, this)
            << new Filetype("c", true, NULL, this)
            << new Filetype("caml", true, NULL, this)
            << new Filetype("changelog", true, NULL, this)
            << new Filetype("clipper", true, NULL, this)
            << new Filetype("cobol", true, NULL, this)
            << new Filetype("coffeescript", true, NULL, this)
            << new Filetype("conf", true, NULL, this)
            << new Filetype("cpp", true, NULL, this)
            << new Filetype("csharp", true, NULL, this)
            << new Filetype("css", true, NULL, this)
            << new Filetype("d", true, NULL, this)
            << new Filetype("desktop", true, NULL, this)
            << new Filetype("diff", true, NULL, this)
            << new Filetype("erlang", true, NULL, this)
            << new Filetype("errors", true, NULL, this)
            << new Filetype("fixed-fortran", true, NULL, this)
            << new Filetype("flex", true, NULL, this)
            << new Filetype("fortran", true, NULL, this)
            << new Filetype("glsl", true, NULL, this)
            << new Filetype("haskell", true, NULL, this)
            << new Filetype("haskell_literate", true, NULL, this)
            << new Filetype("haxe", true, NULL, this)
            << new Filetype("html", true,
                    new WebRunProfileManager(WebRunProfile::Html), this)
            << new Filetype("islisp", true, NULL, this)
            << new Filetype("java", true, NULL, this)
            << new Filetype("javalog", true, NULL, this)
            << new Filetype("javascript", true,
                    new WebRunProfileManager(WebRunProfile::Javascript), this)
            << new Filetype("langdef", true, NULL, this)
            << new Filetype("latex", true, NULL, this)
            << new Filetype("ldap", true, NULL, this)
            << new Filetype("lilypond", true, NULL, this)
            << new Filetype("lisp", true, NULL, this)
            << new Filetype("log", true, NULL, this)
            << new Filetype("logtalk", true, NULL, this)
            << new Filetype("lsm", true, NULL, this)
            << new Filetype("lua", true, NULL, this)
            << new Filetype("m4", true, NULL, this)
            << new Filetype("makefile", true, NULL, this)
            << new Filetype("manifest", true, NULL, this)
            << new Filetype("opa", true, NULL, this)
            << new Filetype("outlang", true, NULL, this)
            << new Filetype("oz", true, NULL, this)
            << new Filetype("pascal", true, NULL, this)
            << new Filetype("pc", true, NULL, this)
            << new Filetype("perl", true, NULL, this)
            << new Filetype("php", true, NULL, this)
            << new Filetype("po", true, NULL, this)
            << new Filetype("postscript", true, NULL, this)
            << new Filetype("prolog", true, NULL, this)
            << new Filetype("properties", true, NULL, this)
            << new Filetype("proto", true, NULL, this)
            << new Filetype("python", true,
                    new CmdRunProfileManager("cd '%2'; /base/usr/bin/python3.2 '%3'"),
                    this)
            << new Filetype("ruby", true, NULL, this)
            << new Filetype("scala", true, NULL, this)
            << new Filetype("scheme", true, NULL, this)
            << new Filetype("sh", true,
                    new CmdRunProfileManager("cd '%2'; /bin/sh '%3'"),
                    this)
            << new Filetype("slang", true, NULL, this)
            << new Filetype("sml", true, NULL, this)
            << new Filetype("spec", true, NULL, this)
            << new Filetype("sql", true, NULL, this)
            << new Filetype("style", true, NULL, this)
            << new Filetype("tcl", true, NULL, this)
            << new Filetype("texinfo", true, NULL, this)
            << new Filetype("tml", true, NULL, this)
            << new Filetype("upc", true, NULL, this)
            << new Filetype("vala", true, NULL, this)
            << new Filetype("vbscript", true, NULL, this)
            << new Filetype("xml", true,
                    new WebRunProfileManager(WebRunProfile::Html), this)
            << new Filetype("xorg", true, NULL, this);
        qDebug() << "Starting to insert filetypes into settings...";
        QList<Filetype *> filetypes = map->filetypes();
        for (int i = 0; i < filetypes.size(); i++) {
            insertFiletype(filetypes[i]);
        }
    } else {
        qDebug() << "Reading filetypes...";
        for (int i = 0; i < keys.size(); i++) {
            RunProfileManager *m = NULL;
            _settings.beginGroup(keys[i]);
            _settings.beginGroup("run_profile_manager");
            switch (_settings.value("type").toInt()) {
                case RunProfileManager::Cmd: {
                    CmdRunProfileManager *cm = new CmdRunProfileManager(_settings.value("cmd").toString());
                    connectCmdRunProfileManager(cm);
                    m = cm;
                    break;
                }
                case RunProfileManager::Web: {
                    WebRunProfileManager *wm = new WebRunProfileManager(
                            (WebRunProfile::Mode) _settings.value("mode").toInt());
                    connectWebRunProfileManager(wm);
                    m = wm;
                    break;
                }
            }
            _settings.endGroup();
            Filetype *filetype = new Filetype(keys[i],
                    _settings.value("highlight_enabled").toBool(),
                    m, this);
            _settings.endGroup();
            connectFiletype(filetype);
            map->add(filetype);
        }
    }
    return map;
}
