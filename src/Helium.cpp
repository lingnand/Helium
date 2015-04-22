/*
 * Application.cpp
 *
 *  Created on: Apr 22, 2015
 *      Author: lingnan
 */

#include <Utility.h>
#include <Helium.h>
#include <View.h>
#include <Buffer.h>
#include <BufferState.h>
#include <HtmlBufferChangeParser.h>

Helium::Helium(int &argc, char **argv): bb::cascades::Application(argc, argv)
{
    qRegisterMetaType<BufferState>("BufferState&");
    qRegisterMetaType<BufferStateChange>();
    qRegisterMetaType<ParserPosition>();

    conn(&_localeHandler, SIGNAL(systemLanguageChanged()),
         this, SLOT(reloadTranslator()));
    conn(this, SIGNAL(translatorChanged()),
         &_rootPane, SLOT(onTranslatorChanged()));

    _rootPane.addNewView();

    reloadTranslator();
    setScene(&_rootPane);
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
