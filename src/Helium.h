/*
 * Application.h
 *
 *  Created on: Apr 22, 2015
 *      Author: lingnan
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <QTranslator>
#include <bb/cascades/Application>
#include <bb/cascades/LocaleHandler>
#include <bb/cascades/ProgressIndicatorState>

namespace bb {
    namespace cascades {
        class ActionItem;
    }
}

class MultiViewPane;
class FiletypeMap;
class GeneralSettings;
class AppearanceSettings;
class SettingsPage;
class HelpPage;
class RepushablePage;

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
    Q_SLOT void reloadTranslator();
Q_SIGNALS:
    void translatorChanged();
private:
    void onTranslatorChanged();
    // settings
    FiletypeMap *_filetypeMap;
    GeneralSettings *_general;
    AppearanceSettings *_appearance;
    SettingsPage *_settingsPage;
    HelpPage *_helpPage;
    bb::cascades::ActionItem *_contactAction;
    Q_SLOT void onContactActionTriggered();

    QTranslator _translator;
    bb::cascades::LocaleHandler _localeHandler;

    MultiViewPane *scene() const {
        return (MultiViewPane *) bb::cascades::Application::scene();
    }
    Q_SLOT void showSettings();
    Q_SLOT void showHelp();
    void pushPage(RepushablePage *);
};

Q_DECLARE_METATYPE(bb::cascades::ProgressIndicatorState::Type)

#endif /* APPLICATION_H_ */
