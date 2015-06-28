/*
 * Application.h
 *
 *  Created on: Apr 22, 2015
 *      Author: lingnan
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <QTranslator>
#include <bb/system/InvokeManager>
#include <bb/system/SystemUiResult>
#include <bb/cascades/Application>
#include <bb/cascades/LocaleHandler>
#include <bb/platform/bbm/Context>

namespace bb {
    namespace cascades {
        class ActionItem;
        class SceneCover;
    }
}

class MultiViewPane;
class FiletypeMap;
class GeneralSettings;
class AppearanceSettings;
class SettingsPage;
class HelpPage;
class RepushablePage;
class Segment;
class View;

class Helium : public bb::cascades::Application
{
    Q_OBJECT
public:
    static Helium *instance();
    Helium(int &argc, char **argv);
    virtual ~Helium() {}
    FiletypeMap *filetypeMap() { return _filetypeMap; }
    GeneralSettings *general() { return _general; }
    AppearanceSettings *appearance() { return _appearance; }
    MultiViewPane *scene() const {
        return (MultiViewPane *) bb::cascades::Application::scene();
    }
    Q_SLOT void reloadTranslator();
Q_SIGNALS:
    void translatorChanged();
private:
    // invocation
    bb::system::InvokeManager _invokeManager;
    Q_SLOT void onInvoked(const bb::system::InvokeRequest &);

    void onTranslatorChanged();
    // settings
    FiletypeMap *_filetypeMap;
    GeneralSettings *_general;
    AppearanceSettings *_appearance;
    SettingsPage *_settingsPage;
    HelpPage *_helpPage;
    bb::cascades::ActionItem *_contactAction;
    Q_SLOT void contact();

    QTranslator _translator;
    bb::cascades::LocaleHandler _localeHandler;

    Q_SLOT void showSettings();
    Q_SLOT void showHelp();
    void pushPage(RepushablePage *);

    // scene cover
    Segment *_coverContent;
    bb::cascades::SceneCover *_cover;
    Q_SLOT void onThumbnail();

    Q_SLOT void onSupportDialogConfirmed(bb::system::SystemUiResult::Type);
    // bbm
    bb::platform::bbm::Context _bbmContext;
    Q_SLOT void onRegistrationStateUpdated(bb::platform::bbm::RegistrationState::Type);
    bb::cascades::ActionItem *_inviteToDownloadAction;
    Q_SLOT void inviteToDownload();
    bb::cascades::ActionItem *_shareAction;
    Q_SLOT void sharePersonalMessage();
    Q_SLOT void onPersonalMessageConfirmed(bb::system::SystemUiResult::Type, const QString &);
};

#endif /* APPLICATION_H_ */
