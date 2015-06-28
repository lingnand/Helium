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
#include <bb/cascades/SceneCover>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/UIPalette>
#include <bb/cascades/Label>
#include <bb/cascades/TextArea>
#include <bb/platform/bbm/MessageService>
#include <bb/platform/bbm/UserProfile>
#include <Segment.h>
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
#include <Segment.h>
#include <Defaults.h>

#define SCENE_COVER_LINE_LIMIT 10

#define PROFILE_BOX_ICON_ID_HELIUM 2

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
        .onTriggered(this, SLOT(contact()))),
    _coverContent(Segment::create().section().subsection()),
    // bbm
    _inviteToDownloadAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_bbm.png"))
        .onTriggered(this, SLOT(inviteToDownload()))),
    _shareAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_share.png"))
        .onTriggered(this, SLOT(sharePersonalMessage()))),
    _bbmContext(QUuid("0dc2bb7b-f557-425f-bc15-58fc504fa7d6"))
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

    setMenu(Menu::create()
        .help(HelpActionItem::create()
            .onTriggered(this, SLOT(showHelp())))
        .settings(SettingsActionItem::create()
            .onTriggered(this, SLOT(showSettings())))
        .addAction(_contactAction)
        .addAction(_inviteToDownloadAction)
        .addAction(_shareAction));

    setCover(_cover = SceneCover::create().content(_coverContent));
    conn(this, SIGNAL(thumbnail()), this, SLOT(onThumbnail()));

    // bbm
    conn(&_bbmContext, SIGNAL(registrationStateUpdated(bb::platform::bbm::RegistrationState::Type)),
        this, SLOT(onRegistrationStateUpdated(bb::platform::bbm::RegistrationState::Type)));

    // set up invocation
    conn(&_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)),
        this, SLOT(onInvoked(const bb::system::InvokeRequest&)));
    switch (_invokeManager.startupMode()) {
        case bb::system::ApplicationStartupMode::InvokeApplication:
            break; // do not add a view as we are about to add one
        default:
            scene()->addNewView(false);
    }

   if (general()->numberOfTimesLaunched() == 1) {
       Utility::dialog(tr("Show Help"),
               tr("Welcome"), tr("Thanks for purchasing Helium!"),
               this, SLOT(showHelp()));
   }
}

void Helium::onRegistrationStateUpdated(bb::platform::bbm::RegistrationState::Type state)
{
    switch (state) {
        case bb::platform::bbm::RegistrationState::Unregistered:
            // request registration if necessary
            _bbmContext.requestRegisterApplication();
            break;
        case bb::platform::bbm::RegistrationState::Allowed:
            if (general()->numberOfTimesLaunched() >= 3 && !general()->hasConfirmedSupport()) {
                Utility::dialog(tr("Leave feedback"), tr("Never ask again"), tr("Share"),
                        tr("Like Helium?"),
                        tr("If yes, help spread the word!"),
                        this, SLOT(onSupportDialogConfirmed(bb::system::SystemUiResult::Type)));
            }
            break;
    }
}

void Helium::onSupportDialogConfirmed(bb::system::SystemUiResult::Type type)
{
    general()->confirmSupport();
    switch (type) {
    case bb::system::SystemUiResult::ConfirmButtonSelection:
        // TODO: go to app world's page
        break;
    case bb::system::SystemUiResult::CustomButtonSelection:
        sharePersonalMessage(); break;
    }
}

void Helium::inviteToDownload()
{
    bb::platform::bbm::MessageService(&_bbmContext).sendDownloadInvitation();
}

void Helium::sharePersonalMessage()
{
    Utility::prompt(tr("OK"), tr("Cancel"),
            tr("Post BBM personal message"),
            tr("I love the wizardry of syntax highlight!"), tr("Enter personal message"),
            this, SLOT(onPersonalMessageConfirmed(bb::system::SystemUiResult::Type, const QString&)));
}

void Helium::onPersonalMessageConfirmed(bb::system::SystemUiResult::Type type, const QString &message)
{
    if (type == bb::system::SystemUiResult::ConfirmButtonSelection) {
        bb::platform::bbm::UserProfile(&_bbmContext).requestUpdatePersonalMessage(message);
    }
}

void Helium::onInvoked(const bb::system::InvokeRequest &request)
{
    // pop any views on the stack
    NavigationPane *pane = scene()->activePane();
    if (pane) {
        pane->navigateTo(pane->at(0));
    }
    // notify the scene to recover from non-view mode
    // TODO: make this less hacky
    View *lastActive = scene()->lastActiveView();
    if (lastActive) {
        lastActive->setNormalMode();
    }
    QString filepath = request.uri().toLocalFile();
    Buffer *buffer = scene()->bufferForFilepath(filepath);
    if (!buffer) {
        buffer = scene()->newBuffer();
        buffer->load(filepath);
    }
    View *v = new View(buffer);
    scene()->insertView(scene()->count(), v);
    scene()->setActiveTab(v, true);
}

void Helium::onThumbnail()
{
    View *view = scene()->lastActiveView();
    if (!view)
        return;
    _cover->setDescription(view->title());
    _coverContent->removeAll();
    _coverContent->setBackground(scene()->ui()->palette()->background());
    const BufferState &state = view->buffer()->state();
    // if buffer is empty then we can use a default label
    if (state.empty()) {
        _coverContent->add(Label::create().text(tr("No content"))
            .textStyle(Defaults::hintText()));
        return;
    }
    // get the texts around the cursorline
    Range range(state.focus(view->textArea()->editor()->cursorPosition()).lineIndex);
    range.from -= SCENE_COVER_LINE_LIMIT/2;
    range.to += SCENE_COVER_LINE_LIMIT/2;
    if (range.to > state.size()) {
        range.from = qMax(state.size()-SCENE_COVER_LINE_LIMIT, 0);
        range.to = state.size();
    } else if (range.from < 0) {
        range.from = 0;
        range.to = qMin(SCENE_COVER_LINE_LIMIT, state.size());
    }
    if (state.highlightType().shouldHighlight()) {
        for (int i = range.from; i < range.to; i++)
            _coverContent->add(Label::create().format(TextFormat::Html)
                    .topMargin(0).bottomMargin(0)
                    .text(state.at(i).highlightText));
    } else {
        for (int i = range.from; i < range.to; i++)
            _coverContent->add(Label::create()
                    .topMargin(0).bottomMargin(0)
                    .text(state.at(i).line.plainText()));
    }
}

void Helium::contact()
{
    bb::system::InvokeRequest request;
    request.setTarget("sys.pim.uib.email.hybridcomposer");
    request.setAction("bb.action.OPEN.bb.action.SENDEMAIL");
    QUrl mailto("mailto:lingnan.d@gmail.com");
    mailto.addQueryItem("subject", tr("RE: Support - Helium %1").arg(bb::ApplicationInfo().version()));
    request.setUri(mailto);
    _invokeManager.invoke(request);
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
    _inviteToDownloadAction->setTitle(tr("Invite"));
    _shareAction->setTitle(tr("Share"));
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
