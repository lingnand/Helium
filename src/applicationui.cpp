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

#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/TabbedPane>
#include <bb/cascades/LocaleHandler>
#include <bb/cascades/Tab>
#include <bb/system/SystemToast>
#include <bb/system/SystemUiPosition>
#include <src/View.h>
#include <src/Buffer.h>

using namespace bb::cascades;

/* member functions */

ApplicationUI::ApplicationUI() :
        QObject()
{
    // prepare the localization
    _translator = new QTranslator(this);
    _localeHandler = new LocaleHandler(this);
    conn(_localeHandler, SIGNAL(systemLanguageChanged()),
         this, SLOT(onSystemLanguageChanged()));

    // Create the root TabbedPane
    _rootPane = new TabbedPane;
    _rootPane->setShowTabsOnActionBar(false);
    // set up the openFileTab
    _newFileTab = new Tab;
    conn(_newFileTab, SIGNAL(triggered()),
         this, SLOT(newFile()));
    _rootPane->add(_newFileTab);
    // set up the openFileTab
    _openFileTab = new Tab;
    conn(_openFileTab, SIGNAL(triggered()),
         this, SLOT(openFile()));
    _rootPane->add(_openFileTab);
    // create a single buffer
    newFile();

    // label initial load
    onSystemLanguageChanged();

    // Set the application scene
    Application::instance()->setScene(_rootPane);
}

void ApplicationUI::newFile() {
    View *view = new View(new Buffer);
    addView(view);
    _rootPane->setActiveTab(view);
}

void ApplicationUI::openFile() {
    bb::system::SystemToast *toast = new bb::system::SystemToast(this);
    toast->setBody("Open file");
    toast->setPosition(bb::system::SystemUiPosition::MiddleCenter);
    toast->show();
}

void ApplicationUI::addView(View* view) {
    _rootPane->add(view);
    _views.append(view);
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(_translator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("Char_%1").arg(locale_string);
    if (_translator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(_translator);
        // new file tab
        _newFileTab->setTitle(tr("New File"));
        // open file tab
        _openFileTab->setTitle(tr("Open File"));
        // loop through all the views to reset the texts
        for (int i=0; i<_views.count(); ++i) {
            _views[i]->onLanguageChanged();
        }
    }
}
