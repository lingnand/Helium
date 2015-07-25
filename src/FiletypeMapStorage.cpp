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

void FiletypeMapStorage::connectFiletype(Filetype *filetype)
{
    conn(filetype, SIGNAL(highlightEnabledChanged(bool)),
            this, SLOT(onFiletypeHighlightEnabledChanged(bool)));
    conn(filetype, SIGNAL(tabSpaceConversionEnabledChanged(bool)),
            this, SLOT(onFiletypeTabSpaceConversionEnabledChanged(bool)));
    conn(filetype, SIGNAL(numberOfSpacesForTabChanged(int)),
            this, SLOT(onFiletypeNumberOfSpacesForTabChanged(int)));
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

void FiletypeMapStorage::onFiletypeTabSpaceConversionEnabledChanged(bool enabled)
{
    Filetype *f = (Filetype *) sender();
    _settings.setValue(f->name()+"/tab_space_conversion_enabled", enabled);
}

void FiletypeMapStorage::onFiletypeNumberOfSpacesForTabChanged(int number)
{
    Filetype *f = (Filetype *) sender();
    _settings.setValue(f->name()+"/number_of_spaces_for_tab", number);
}

FiletypeMap *FiletypeMapStorage::read()
{
    FiletypeMap *map = new FiletypeMap(this);
    qDebug() << "Reading filetypes...";
    QStringList keys = _settings.childGroups();
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
                _settings.value("tab_space_conversion_enabled").toBool(),
                _settings.value("number_of_spaces_for_tab", 4).toInt(),
                m, this);
        _settings.endGroup();
        connectFiletype(filetype);
        map->add(filetype);
    }
    QList<Filetype *> defaults;
    defaults << new Filetype("ada", true, false, 4, NULL, this)
        << new Filetype("applescript", true, false, 4, NULL, this)
        << new Filetype("asm", true, false, 4, NULL, this)
        << new Filetype("awk", true, false, 4, NULL, this)
        << new Filetype("bat", true, false, 4, NULL, this)
        << new Filetype("bib", true, false, 4, NULL, this)
        << new Filetype("bison", true, false, 4, NULL, this)
        << new Filetype("c", true, false, 4, NULL, this)
        << new Filetype("caml", true, false, 4, NULL, this)
        << new Filetype("changelog", true, false, 4, NULL, this)
        << new Filetype("clipper", true, false, 4, NULL, this)
        << new Filetype("cobol", true, false, 4, NULL, this)
        << new Filetype("coffeescript", true, false, 4, NULL, this)
        << new Filetype("conf", true, false, 4, NULL, this)
        << new Filetype("cpp", true, false, 4, NULL, this)
        << new Filetype("csharp", true, false, 4, NULL, this)
        << new Filetype("css", true, false, 4, NULL, this)
        << new Filetype("d", true, false, 4, NULL, this)
        << new Filetype("desktop", true, false, 4, NULL, this)
        << new Filetype("diff", true, false, 4, NULL, this)
        << new Filetype("erlang", true, false, 4, NULL, this)
//            << new Filetype("errors", true, false, 4, NULL, this)
//            << new Filetype("fixed-fortran", true, false, 4, NULL, this)
        << new Filetype("flex", true, false, 4, NULL, this)
        << new Filetype("fortran", true, false, 4, NULL, this)
        << new Filetype("glsl", true, false, 4, NULL, this)
        << new Filetype("haskell", true, false, 4, NULL, this)
        << new Filetype("haskell_literate", true, false, 4, NULL, this)
        << new Filetype("haxe", true, false, 4, NULL, this)
        << new Filetype("html", true, false, 4,
                new WebRunProfileManager(WebRunProfile::Html), this)
//            << new Filetype("islisp", true, false, 4, NULL, this)
        << new Filetype("java", true, false, 4, NULL, this)
//            << new Filetype("javalog", true, false, 4, NULL, this)
        << new Filetype("javascript", true, false, 4,
                new WebRunProfileManager(WebRunProfile::Javascript), this)
//            << new Filetype("langdef", true, false, 4, NULL, this)
        << new Filetype("latex", true, false, 4, NULL, this)
        << new Filetype("ldap", true, false, 4, NULL, this)
        << new Filetype("lilypond", true, false, 4, NULL, this)
        << new Filetype("lisp", true, false, 4, NULL, this)
        << new Filetype("log", true, false, 4, NULL, this)
        << new Filetype("logtalk", true, false, 4, NULL, this)
//            << new Filetype("lsm", true, false, 4, NULL, this)
        << new Filetype("lua", true, false, 4, NULL, this)
        << new Filetype("m4", true, false, 4, NULL, this)
        << new Filetype("makefile", true, false, 4, NULL, this)
        << new Filetype("manifest", true, false, 4, NULL, this)
//            << new Filetype("opa", true, false, 4, NULL, this)
//            << new Filetype("outlang", true, false, 4, NULL, this)
//            << new Filetype("oz", true, false, 4, NULL, this)
        << new Filetype("pascal", true, false, 4, NULL, this)
//            << new Filetype("pc", true, false, 4, NULL, this)
        << new Filetype("perl", true, false, 4, NULL, this)
        << new Filetype("php", true, false, 4, NULL, this)
//            << new Filetype("po", true, false, 4, NULL, this)
        << new Filetype("postscript", true, false, 4, NULL, this)
        << new Filetype("prolog", true, false, 4, NULL, this)
//            << new Filetype("properties", true, false, 4, NULL, this)
//            << new Filetype("proto", true, false, 4, NULL, this)
        << new Filetype("python", true, false, 4,
                new CmdRunProfileManager("cd '%dir%'; exec /base/usr/bin/python3.2 -u '%name%'"),
                this)
        << new Filetype("ruby", true, false, 4, NULL, this)
        << new Filetype("scala", true, false, 4, NULL, this)
        << new Filetype("scheme", true, false, 4, NULL, this)
        << new Filetype("sh", true, false, 4,
                new CmdRunProfileManager("cd '%dir%'; exec /bin/sh '%name%'"),
                this)
        << new Filetype("slang", true, false, 4, NULL, this)
        << new Filetype("sml", true, false, 4, NULL, this)
//            << new Filetype("spec", true, false, 4, NULL, this)
        << new Filetype("sql", true, false, 4, NULL, this)
//            << new Filetype("style", true, false, 4, NULL, this)
        << new Filetype("tcl", true, false, 4, NULL, this)
        << new Filetype("texinfo", true, false, 4, NULL, this)
//            << new Filetype("tml", true, false, 4, NULL, this)
//            << new Filetype("upc", true, false, 4, NULL, this)
//            << new Filetype("vala", true, false, 4, NULL, this)
        << new Filetype("vbscript", true, false, 4, NULL, this)
        << new Filetype("xml", true, false, 4,
                new WebRunProfileManager(WebRunProfile::Html), this)
        << new Filetype("markdown", true, false, 4,
                new WebRunProfileManager(WebRunProfile::Markdown), this);
//            << new Filetype("xorg", true, NULL, this);
    for (int i = 0; i < defaults.size(); i++) {
        Filetype *filetype = defaults[i];
        if (!map->filetype(filetype->name())) {
            map->add(filetype);
            // the reason to insert here is because when user makes changes
            // these are recorded as individual fields in the settings
            // if we don't insert the default then next time on startup we
            // are going to load up a bunch of filetypes with half-complete settings
            qDebug() << "inserting filetype" << filetype->name();
            _settings.setValue(filetype->name()+"/highlight_enabled", filetype->highlightEnabled());
            insertRunProfileManager(filetype->runProfileManager());
            connectFiletype(filetype);
        }
    }
    return map;
}
