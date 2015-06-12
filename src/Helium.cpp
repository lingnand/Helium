/*
 * Application.cpp
 *
 *  Created on: Apr 22, 2015
 *      Author: lingnan
 */

#include <bb/ApplicationInfo>
#include <bb/system/InvokeManager>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/ThemeSupport>
#include <bb/cascades/Page>
#include <bb/cascades/Menu>
#include <bb/cascades/ActionItem>
#include <bb/cascades/SettingsActionItem>
#include <bb/cascades/HelpActionItem>
#include <Helium.h>
#include <MultiViewPane.h>
#include <View.h>
#include <StateChangeContext.h>
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
    _helpPage(NULL),
    _contactAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_email.png"))
        .onTriggered(this, SLOT(onContactActionTriggered())))
{
    themeSupport()->setVisualStyle(_appearance->visualStyle());
    conn(_appearance, SIGNAL(visualStyleChanged(bb::cascades::VisualStyle::Type)),
        themeSupport(), SLOT(setVisualStyle(bb::cascades::VisualStyle::Type)));

    reloadTranslator();
    conn(&_localeHandler, SIGNAL(systemLanguageChanged()),
         this, SLOT(reloadTranslator()));

    setScene(new MultiViewPane(this));
    conn(this, SIGNAL(translatorChanged()),
         scene(), SLOT(onTranslatorChanged()));
    scene()->addNewView(false);

    setMenu(bb::cascades::Menu::create()
        .help(HelpActionItem::create()
            .onTriggered(this, SLOT(showHelp())))
        .settings(SettingsActionItem::create()
            .onTriggered(this, SLOT(showSettings())))
        .addAction(_contactAction));
}

void Helium::onContactActionTriggered()
{
    bb::system::InvokeRequest request;
    request.setTarget("sys.pim.uib.email.hybridcomposer");
    request.setAction("bb.action.OPEN.bb.action.SENDEMAIL");
    QUrl mailto("mailto:lingnan.d@gmail.com");
    bb::ApplicationInfo info;
    mailto.addQueryItem("subject", tr("RE: Support - Helium %1").arg(info.version()));
    request.setUri(mailto);
    bb::system::InvokeManager().invoke(request);
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

void Helium::onTranslatorChanged()
{
    _contactAction->setTitle(tr("Contact"));
    emit translatorChanged();
}

void Helium::reloadTranslator()
{
    QCoreApplication::instance()->removeTranslator(&_translator);
    // Initiate, load and install the application translation files.
    if (_translator.load("Char_" + QLocale().name(), "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(&_translator);
        onTranslatorChanged();
    }
}
