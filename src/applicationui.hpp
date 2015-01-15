/*
 * Copyright (c) 2011-2014 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>
#include <QList>

namespace bb
{
    namespace cascades
    {
        class LocaleHandler;
        class TabbedPane;
        class Tab;
    }
}

class QTranslator;
class View;

/*!
 * @brief Application UI object
 *
 * Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class ApplicationUI : public QObject
{
    Q_OBJECT
public:
    ApplicationUI();
    virtual ~ApplicationUI() {}
private:
    QTranslator* _translator;
    bb::cascades::LocaleHandler* _localeHandler;
    Q_SLOT void onSystemLanguageChanged();

    bb::cascades::TabbedPane* _rootPane;

    bb::cascades::Tab* _openFileTab;
    bb::cascades::Tab* _newViewTab;
    Q_SLOT void openFile();
    Q_SLOT void newView();
    Q_SLOT void prevView();
    Q_SLOT void nextView();
    Q_SLOT void activateViewWithOffset(int offset);
    void appendNewView();
};

#endif /* ApplicationUI_HPP_ */
