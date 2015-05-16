/*
 * Application.cpp
 *
 *  Created on: Apr 22, 2015
 *      Author: lingnan
 */

#include <Helium.h>
#include <MultiViewPane.h>
#include <View.h>
#include <Buffer.h>
#include <BufferState.h>
#include <StateChangeContext.h>
#include <HtmlBufferChangeParser.h>
#include <Filetype.h>
#include <CmdRunProfileManager.h>
#include <Utility.h>

Helium *Helium::instance()
{
    return (Helium *) bb::cascades::Application::instance();
}

Helium::Helium(int &argc, char **argv):
        bb::cascades::Application(argc, argv)
{
    qRegisterMetaType<bb::cascades::ProgressIndicatorState::Type>();
    qRegisterMetaType<BufferState>("BufferState&");
    qRegisterMetaType<Progress>("Progress&");
    qRegisterMetaType<BufferStateChange>();
    qRegisterMetaType<StateChangeContext>("StateChangeContext&");
    qRegisterMetaType<ParserPosition>();

    // setttings
    // populate the filetypeMap
    _filetypeMap << new Filetype("ada", true, NULL, this)
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
        << new Filetype("html", true, NULL, this)
        << new Filetype("islisp", true, NULL, this)
        << new Filetype("java", true, NULL, this)
        << new Filetype("javalog", true, NULL, this)
        << new Filetype("javascript", true, NULL, this)
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
        << new Filetype("xml", true, NULL, this)
        << new Filetype("xorg", true, NULL, this);

    reloadTranslator();
    conn(&_localeHandler, SIGNAL(systemLanguageChanged()),
         this, SLOT(reloadTranslator()));

    _rootPane = new MultiViewPane(this);
    conn(this, SIGNAL(translatorChanged()),
         _rootPane, SLOT(onTranslatorChanged()));

    _rootPane->addNewView(false);

    setScene(_rootPane);
}

void Helium::reloadTranslator()
{
    QCoreApplication::instance()->removeTranslator(&_translator);
    // Initiate, load and install the application translation files.
    if (_translator.load("Char_" + QLocale().name(), "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(&_translator);
        emit translatorChanged();
    }
}

FiletypeMap *Helium::filetypeMap()
{
    return &_filetypeMap;
}
