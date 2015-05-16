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
#include <FiletypeMap.h>

class MultiViewPane;

class Helium : public bb::cascades::Application
{
    Q_OBJECT
public:
    static Helium *instance();
    Helium(int &argc, char **argv);
    virtual ~Helium() {}
    FiletypeMap *filetypeMap();
    Q_SLOT void reloadTranslator();
Q_SIGNALS:
    void translatorChanged();
private:
    // settings
    FiletypeMap _filetypeMap;

    MultiViewPane *_rootPane;
    QTranslator _translator;
    bb::cascades::LocaleHandler _localeHandler;
};

Q_DECLARE_METATYPE(bb::cascades::ProgressIndicatorState::Type)

#endif /* APPLICATION_H_ */
