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
#include <Utility.h>

Helium::Helium(int &argc, char **argv): bb::cascades::Application(argc, argv)
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
