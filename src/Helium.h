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
#include <HelpPage.h>
#include <Versioner.h>

namespace bb {
    namespace cascades {
        class ActionItem;
        class SceneCover;
        class Sheet;
        class AbstractPane;
    }
}

class MultiViewPane;
class FiletypeMap;
class GeneralSettings;
class AppearanceSettings;
class GitSettings;
class SettingsPage;
class Segment;
class View;
class BufferStore;

class Helium : public bb::cascades::Application
{
    Q_OBJECT
public:
    static Helium *instance();
    Helium(int &argc, char **argv);
    virtual ~Helium() {}
    const Versioner &version() { return _version; }
    FiletypeMap *filetypeMap() { return _filetypeMap; }
    GeneralSettings *general() { return _general; }
    AppearanceSettings *appearance() { return _appearance; }
    GitSettings *git() { return _git; }
    BufferStore *buffers() { return _buffers; }
    MultiViewPane *scene() const {
        return (MultiViewPane *) bb::cascades::Application::scene();
    }
    Q_SLOT void reloadTranslator();
    Q_SLOT void goToAppWorld();
    bool notify(QObject *, QEvent *);
Q_SIGNALS:
    void translatorChanged();
private:
    BufferStore *_buffers;
    bb::cascades::Sheet *_sheet;
    SettingsPage *_settingsPage;
    HelpPage *_helpPage;

    Segment *_coverContent;
    bb::cascades::SceneCover *_cover;

    bb::cascades::ActionItem *_contactAction;
    bb::cascades::ActionItem *_shareAction;
    bb::cascades::ActionItem *_fullScreenAction;
    bb::platform::bbm::Context _bbmContext;

    FiletypeMap *_filetypeMap;
    GeneralSettings *_general;
    AppearanceSettings *_appearance;
    GitSettings *_git;

    Versioner _version;
    bb::system::InvokeManager _invokeManager;
    QTranslator _translator;
    bb::cascades::LocaleHandler _localeHandler;

    void performUpdate(const Version &current, const Version &last);
    Q_SLOT void toggleFullScreen();
    Q_SLOT void showSettings();
    Q_SLOT void showHelp(HelpPage::Mode mode=HelpPage::Reference);
    Q_SLOT void contact();
    Q_SLOT void share();
    Q_SLOT void inviteToDownload();
    Q_SLOT void postPersonalMessage();
    Q_SLOT void resetFullScreenAction();
    void pushOnSheet(bb::cascades::AbstractPane *);

    void onTranslatorChanged();
    Q_SLOT void onInvoked(const bb::system::InvokeRequest &);
    Q_SLOT void onThumbnail();
    Q_SLOT void onFullscreen();
    Q_SLOT void onShareDialogConfirmed(bb::system::SystemUiResult::Type);
    Q_SLOT void onSupportDialogConfirmed(bb::system::SystemUiResult::Type);
    Q_SLOT void onRegistrationStateUpdated(bb::platform::bbm::RegistrationState::Type);
    Q_SLOT void onPersonalMessageConfirmed(bb::system::SystemUiResult::Type, const QString &);
};

#endif /* APPLICATION_H_ */
