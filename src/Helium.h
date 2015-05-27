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

class MultiViewPane;
class FiletypeMap;
class GeneralSettings;
class SettingsPage;

class Helium : public bb::cascades::Application
{
    Q_OBJECT
public:
    static Helium *instance();
    Helium(int &argc, char **argv);
    virtual ~Helium() {}
    FiletypeMap *filetypeMap() { return _filetypeMap; }
    GeneralSettings *general() { return _general; }
    Q_SLOT void reloadTranslator();
Q_SIGNALS:
    void translatorChanged();
private:
    // settings
    FiletypeMap *_filetypeMap;
    GeneralSettings *_general;
    SettingsPage *_settingsPage;

    QTranslator _translator;
    bb::cascades::LocaleHandler _localeHandler;

    MultiViewPane *scene() const {
        return (MultiViewPane *) bb::cascades::Application::scene();
    }
    Q_SLOT void showSettings();
    Q_SLOT void showHelp();
};

Q_DECLARE_METATYPE(bb::cascades::ProgressIndicatorState::Type)

#endif /* APPLICATION_H_ */
