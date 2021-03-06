/*
 * Application.cpp
 *
 *  Created on: Apr 22, 2015
 *      Author: lingnan
 */

#include <bb/system/InvokeManager>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/ThemeSupport>
#include <bb/cascades/Page>
#include <bb/cascades/Menu>
#include <bb/cascades/ActionItem>
#include <bb/cascades/HelpActionItem>
#include <bb/cascades/SettingsActionItem>
#include <bb/cascades/SceneCover>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/UIPalette>
#include <bb/cascades/Label>
#include <bb/cascades/TextArea>
#include <bb/cascades/Sheet>
#include <bb/platform/bbm/MessageService>
#include <bb/platform/bbm/UserProfile>
#include <libqgit2/qgitexception.h>
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
#include <GitSettings.h>
#include <GitSettingsStorage.h>
#include <SettingsPage.h>
#include <Utility.h>
#include <Segment.h>
#include <Project.h>
#include <Defaults.h>
#include <BufferStore.h>
#include <ProjectStorage.h>

#define SCENE_COVER_LINE_LIMIT 10

#define PROFILE_BOX_ICON_ID_HELIUM 2
#define BBM_UUID "0dc2bb7b-f557-425f-bc15-58fc504fa7d6"
#define APPWORLD_URI "appworld://content/59963952"

using namespace bb::cascades;

Helium *Helium::instance()
{
    return (Helium *) Application::instance();
}

Helium::Helium(int &argc, char **argv):
    Application(argc, argv),
    _buffers(new BufferStore(this)),
    _sheet(Sheet::create()),
    _settingsPage(NULL),
    _helpPage(NULL),
    _coverContent(Segment::create().section().subsection()),
    _cover(SceneCover::create().content(_coverContent)),
    _contactAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_email.png"))
        .onTriggered(this, SLOT(contact()))),
    _shareAction(ActionItem::create()
        .imageSource(QUrl("asset:///images/ic_bbm.png"))
        .onTriggered(this, SLOT(share()))),
    _fullScreenAction(ActionItem::create()
        .onTriggered(this, SLOT(toggleFullScreen()))),
    _bbmContext(QUuid(BBM_UUID))
{
    // settings
    if (!_version.last().isNull())
        performUpdate(_version.current(), _version.last());

    _filetypeMap = (new FiletypeMapStorage("filetypes", this))->read();
    _general = (new GeneralSettingsStorage("general_settings", this))->read();
    _appearance = (new AppearanceSettingsStorage("appearance_settings", this))->read();
    _git = (new GitSettingsStorage("git_settings", this))->read();

    // UI
    reloadTranslator();
    conn(&_localeHandler, SIGNAL(systemLanguageChanged()),
         this, SLOT(reloadTranslator()));

    themeSupport()->setVisualStyle(_appearance->visualStyle());
    conn(_appearance, SIGNAL(visualStyleChanged(bb::cascades::VisualStyle::Type)),
        themeSupport(), SLOT(setVisualStyle(bb::cascades::VisualStyle::Type)));

    conn(_appearance, SIGNAL(fullScreenChanged(bool)),
            this, SLOT(resetFullScreenAction()));

    setScene(new MultiViewPane(
            (new ProjectStorage("projects", this))->read(), this));
    conn(this, SIGNAL(translatorChanged()),
         scene(), SLOT(onTranslatorChanged()));

    setMenu(Menu::create()
        .help(HelpActionItem::create()
            .onTriggered(this, SLOT(showHelp())))
        .settings(SettingsActionItem::create()
            .onTriggered(this, SLOT(showSettings())))
        .addAction(_contactAction)
        .addAction(_shareAction)
        .addAction(_fullScreenAction));

    setCover(_cover);
    conn(this, SIGNAL(thumbnail()), this, SLOT(onThumbnail()));
    conn(this, SIGNAL(fullscreen()), this, SLOT(onFullscreen()));

    // bbm
    conn(&_bbmContext, SIGNAL(registrationStateUpdated(bb::platform::bbm::RegistrationState::Type)),
        this, SLOT(onRegistrationStateUpdated(bb::platform::bbm::RegistrationState::Type)));

    // set up invocation
    conn(&_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)),
        this, SLOT(onInvoked(const bb::system::InvokeRequest&)));

    if (_general->numberOfTimesLaunched() == 1) {
        Utility::dialog(tr("Show Help"),
                tr("Welcome"), tr("Thanks for purchasing Helium! Please review Help for a quick tour of the features. Use \"Contact\" from the swipe-down menu for bug report/feature request."),
                this, SLOT(showHelp()));
    } else if (_version.last() < _version.current()) {
        Utility::toast(tr("Helium updated to %1").arg(_version.current().string()));
        showHelp(HelpPage::ChangeList);
    }
}

void Helium::toggleFullScreen()
{
    _appearance->setFullScreen(!_appearance->fullScreen());
}

void Helium::performUpdate(const Version &, const Version &last)
{
    if (last >= Version(1, 1, 0, 2))
        return;
    // notice for 1.1.0.2
    Utility::dialog("Gotcha", "From Developer", "There's been report of git clone appearing 'stuck'. "
            "This is expected behavior for big repositories due to object unpacking. "
            "The progress indicator might look still but the background worker thread should be running nonstop. "
            "If there is an error the progress indicator will turn red with the message displayed. "
            "While waiting, you can go back to edit other files (the background thread will inform you when it finishes). "
            "Or just copy your repo with .git directory from your PC to the phone.\n"
            "\n"
            "Please use \"Contact\" in the top swipe-down menu for any bug report/feature request (there's no way to answer queries/support problems left in BBW review).\n"
            "\n"
            "Many thanks and happy hacking!");
    if (last >= Version(1, 0, 3, 0))
        return;
    QSettings settings;
    QVariant v = settings.value("general_settings/default_open_directory");
    if (v.isValid()) {
        settings.remove("general_settings/default_open_directory");
        if (v.toString() != "/accounts/1000/shared") // this is an invalid directory
            settings.setValue("general_settings/default_project_directory", v);
    }
    if (last >= Version(1, 0, 2, 4))
        return;
    if (settings.value("filetypes/python/run_profile_manager/cmd").toString()
            == "cd '%dir%'; /base/usr/bin/python3.2 '%name%'")
        settings.setValue("filetypes/python/run_profile_manager/cmd",
                "cd '%dir%'; exec /base/usr/bin/python3.2 -u '%name%'");
    if (settings.value("filetypes/sh/run_profile_manager/cmd").toString()
            == "cd '%dir%'; /bin/sh '%name%'")
        settings.setValue("filetypes/sh/run_profile_manager/cmd",
                "cd '%dir%'; exec /bin/sh '%name%'");
}

void Helium::onRegistrationStateUpdated(bb::platform::bbm::RegistrationState::Type state)
{
    switch (state) {
        case bb::platform::bbm::RegistrationState::Unregistered:
            // request registration if necessary
            _bbmContext.requestRegisterApplication();
            break;
        case bb::platform::bbm::RegistrationState::Allowed:
            if (_general->numberOfTimesLaunched() >= 3 && !_general->hasConfirmedSupport()) {
                Utility::dialog(tr("Rate it"), tr("Never ask again"), tr("Post BBM status"),
                        tr("Like Helium?"), tr("If yes, help spread the word!"),
                        this, SLOT(onSupportDialogConfirmed(bb::system::SystemUiResult::Type)));
            }
            break;
    }
}

void Helium::onSupportDialogConfirmed(bb::system::SystemUiResult::Type type)
{
    _general->confirmSupport();
    switch (type) {
    case bb::system::SystemUiResult::ConfirmButtonSelection: {
        goToAppWorld(); break;
    }
    case bb::system::SystemUiResult::CustomButtonSelection:
        postPersonalMessage(); break;
    }
}

void Helium::goToAppWorld()
{
    bb::system::InvokeRequest request;
    request.setTarget("sys.appworld");
    request.setAction("bb.action.OPEN");
    request.setUri(QUrl(APPWORLD_URI));
    _invokeManager.invoke(request);
}

void Helium::share()
{
    Utility::dialog(tr("Post status"), tr("Cancel"), tr("Invite to download"),
            tr("BBM Share"), tr("Support Helium by spreading the love!"),
            this, SLOT(onShareDialogConfirmed(bb::system::SystemUiResult::Type)));
}

void Helium::onShareDialogConfirmed(bb::system::SystemUiResult::Type type)
{
    _general->confirmSupport();
    switch (type) {
    case bb::system::SystemUiResult::ConfirmButtonSelection:
        postPersonalMessage(); break;
    case bb::system::SystemUiResult::CustomButtonSelection:
        inviteToDownload(); break;
    }
}

void Helium::inviteToDownload()
{
    if (!bb::platform::bbm::MessageService(&_bbmContext).sendDownloadInvitation()) {
        Utility::toast(tr("Failed to send download invites: maybe you haven't granted Helium BBM permissions"));
    }
}

void Helium::postPersonalMessage()
{
    Utility::prompt(tr("OK"), tr("Cancel"),
            tr("Post BBM status message"), tr("This will be visible to your BBM contacts"),
            tr("I love the wizardry of syntax highlight!"), tr("Enter status message"),
            this, SLOT(onPersonalMessageConfirmed(bb::system::SystemUiResult::Type, const QString&)));
}

void Helium::onPersonalMessageConfirmed(bb::system::SystemUiResult::Type type, const QString &message)
{
    if (type == bb::system::SystemUiResult::ConfirmButtonSelection) {
        if (!bb::platform::bbm::UserProfile(&_bbmContext).requestUpdatePersonalMessage(message)) {
            Utility::toast(tr("Failed to update status message: maybe you haven't granted Helium BBM permissions"));
        }
    }
}

void Helium::onInvoked(const bb::system::InvokeRequest &request)
{
    // notify the scene to recover from non-view mode
    // TODO: make this less hacky
    Project *p = scene()->activeProject();
    p->activeView()->setNormalMode();
    p->openFilesAt(p->activeViewIndex(), QStringList(request.uri().toLocalFile()));
}

void Helium::onThumbnail()
{
    View *view = scene()->activeProject()->activeView();
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

void Helium::onFullscreen()
{
    scene()->activeProject()->activeView()->onActivated();
}

void Helium::contact()
{
    bb::system::InvokeRequest request;
    request.setTarget("sys.pim.uib.email.hybridcomposer");
    request.setAction("bb.action.OPEN.bb.action.SENDEMAIL");
    QUrl mailto("mailto:lingnan.d@gmail.com");
    mailto.addQueryItem("subject", tr("RE: Support - Helium %1").arg(
            _version.current().string()));
    request.setUri(mailto);
    _invokeManager.invoke(request);
}

void Helium::pushOnSheet(AbstractPane *page)
{
    if (!_sheet)
        _sheet = Sheet::create();
    _sheet->setContent(page);
    _sheet->open();
}

void Helium::showSettings()
{
    if (!_settingsPage) {
        _settingsPage = new SettingsPage(_general, _appearance, _filetypeMap, _git);
        conn(this, SIGNAL(translatorChanged()),
            _settingsPage, SLOT(onTranslatorChanged()));
    }
    pushOnSheet(_settingsPage);
}

void Helium::showHelp(HelpPage::Mode mode)
{
    if (!_helpPage) {
        _helpPage = new HelpPage;
        conn(this, SIGNAL(translatorChanged()),
            _helpPage, SLOT(onTranslatorChanged()));
    }
    _helpPage->setMode(mode);
    pushOnSheet(_helpPage);
}

void Helium::resetFullScreenAction()
{
    if (_appearance->fullScreen()) {
        _fullScreenAction->setTitle(tr("Exit Full-screen"));
        _fullScreenAction->setImage(QUrl("asset:///images/ic_exit_fullscreen.png"));
    } else {
        _fullScreenAction->setTitle(tr("Enter Full-screen"));
        _fullScreenAction->setImage(QUrl("asset:///images/ic_fullscreen.png"));
    }
}

void Helium::onTranslatorChanged()
{
    _contactAction->setTitle(tr("Contact"));
    _shareAction->setTitle(tr("BBM Share"));
    resetFullScreenAction();
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

bool Helium::notify(QObject *receiver, QEvent *event)
{
    try {
        return Application::notify(receiver, event);
    } catch (const LibQGit2::Exception &e) {
        qDebug() << "::::LIBQGIT2 ERROR::::" << e.what();
    }
    return false;
}
