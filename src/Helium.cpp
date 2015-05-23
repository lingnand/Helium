/*
 * Application.cpp
 *
 *  Created on: Apr 22, 2015
 *      Author: lingnan
 */

#include <bb/cascades/NavigationPane>
#include <bb/cascades/Page>
#include <Helium.h>
#include <MultiViewPane.h>
#include <View.h>
#include <Buffer.h>
#include <BufferState.h>
#include <StateChangeContext.h>
#include <HtmlBufferChangeParser.h>
#include <FiletypeMapStorage.h>
#include <Filetype.h>
#include <CmdRunProfileManager.h>
#include <Utility.h>

Helium *Helium::instance()
{
    return (Helium *) bb::cascades::Application::instance();
}

Helium::Helium(int &argc, char **argv):
        bb::cascades::Application(argc, argv),
        _filetypeMap((new FiletypeMapStorage("filetypes", this))->read())
{
    qRegisterMetaType<bb::cascades::ProgressIndicatorState::Type>();
    qRegisterMetaType<BufferState>("BufferState&");
    qRegisterMetaType<Progress>("Progress&");
    qRegisterMetaType<BufferStateChange>();
    qRegisterMetaType<StateChangeContext>("StateChangeContext&");
    qRegisterMetaType<ParserPosition>();

    reloadTranslator();
    conn(&_localeHandler, SIGNAL(systemLanguageChanged()),
         this, SLOT(reloadTranslator()));

    MultiViewPane *rootPane = new MultiViewPane(this);
    conn(this, SIGNAL(translatorChanged()),
         rootPane, SLOT(onTranslatorChanged()));
    rootPane->addNewView(false);
    setScene(rootPane);
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
    return _filetypeMap;
}
