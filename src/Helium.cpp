/*
 * Application.cpp
 *
 *  Created on: Apr 22, 2015
 *      Author: lingnan
 */

#include <bb/cascades/NavigationPane>
#include <bb/cascades/Page>
#include <bb/cascades/Menu>
#include <bb/cascades/SettingsActionItem>
#include <bb/cascades/HelpActionItem>
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
#include <GeneralSettingsStorage.h>
#include <GeneralSettings.h>
#include <AppearanceSettingsStorage.h>
#include <AppearanceSettings.h>
#include <SettingsPage.h>
#include <HelpPage.h>
#include <Utility.h>
#include <RepushablePage.h>

using namespace bb::cascades;

Helium *Helium::instance()
{
    return (Helium *) Application::instance();
}

Helium::Helium(int &argc, char **argv):
    Application(argc, argv),
    _filetypeMap((new FiletypeMapStorage("filetypes", this))->read()),
    _general((new GeneralSettingsStorage("general_settings", this))->read()),
    _appearance((new AppearanceSettingsStorage("appearance_settings", this))->read()),
    _settingsPage(NULL),
    _helpPage(NULL)
{
    qRegisterMetaType<ProgressIndicatorState::Type>();
    qRegisterMetaType<HighlightType>();
    qRegisterMetaType<BufferState>("BufferState&");
    qRegisterMetaType<Progress>("Progress&");
    qRegisterMetaType<BufferStateChange>();
    qRegisterMetaType<StateChangeContext>("StateChangeContext&");
    qRegisterMetaType<ParserPosition>();

    reloadTranslator();
    conn(&_localeHandler, SIGNAL(systemLanguageChanged()),
         this, SLOT(reloadTranslator()));

    setScene(new MultiViewPane(this));
    conn(this, SIGNAL(translatorChanged()),
         scene(), SLOT(onTranslatorChanged()));
    scene()->addNewView(false);

    setMenu(bb::cascades::Menu::create()
        .settings(SettingsActionItem::create()
            .onTriggered(this, SLOT(showSettings())))
        .help(HelpActionItem::create()
            .onTriggered(this, SLOT(showHelp()))));
}

void Helium::pushPage(RepushablePage *page)
{
    scene()->disableAllShortcuts();
    page->disconnect();
    conn(page, SIGNAL(exited()),
        scene(), SLOT(enableAllShortcuts()));
    NavigationPane *pane = scene()->activePane();
    // pop to the base page
    pane->navigateTo(pane->at(0));
    conn(page, SIGNAL(toPush(bb::cascades::Page*)),
        pane, SLOT(push(bb::cascades::Page*)));
    conn(page, SIGNAL(toPop()),
        pane, SLOT(pop()));
    page->setParent(NULL);
    pane->push(page);
}

void Helium::showSettings()
{
    if (!_settingsPage) {
        _settingsPage = new SettingsPage(_general, _appearance, _filetypeMap, this);
        conn(this, SIGNAL(translatorChanged()),
            _settingsPage, SLOT(onTranslatorChanged()));
    }
    pushPage(_settingsPage);
}

void Helium::showHelp()
{
    if (!_helpPage) {
        _helpPage = new HelpPage(this);
        conn(this, SIGNAL(translatorChanged()),
            _helpPage, SLOT(onTranslatorChanged()));
    }
    pushPage(_helpPage);
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
