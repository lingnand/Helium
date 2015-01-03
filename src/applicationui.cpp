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
#include <src/Helper.h>

using namespace bb::cascades;

/* helpers */

/* member functions */

ApplicationUI::ApplicationUI() :
        QObject()
{
    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);
    conn(m_pLocaleHandler, SIGNAL(systemLanguageChanged()),
         this, SLOT(onSystemLanguageChanged()));

    // Create the root TabbedPane
    m_pRootPane = new TabbedPane;
    m_pRootPane->setShowTabsOnActionBar(false);
    // set up the openFileTab
    m_pNewFileTab = new Tab;
    conn(m_pNewFileTab, SIGNAL(triggered()),
         this, SLOT(newFile()));
    m_pRootPane->add(m_pNewFileTab);
    // set up the openFileTab
    m_pOpenFileTab = new Tab;
    conn(m_pOpenFileTab, SIGNAL(triggered()),
         this, SLOT(openFile()));
    m_pRootPane->add(m_pOpenFileTab);
    // create a single buffer
    newFile();

    // label initial load
    onSystemLanguageChanged();

    // Set the application scene
    Application::instance()->setScene(m_pRootPane);
}

void ApplicationUI::newFile() {
    View *view = new View(new Buffer);
    addView(view);
    m_pRootPane->setActiveTab(view);
}

void ApplicationUI::openFile() {
    bb::system::SystemToast *toast = new bb::system::SystemToast(this);
    toast->setBody("Open file");
    toast->setPosition(bb::system::SystemUiPosition::MiddleCenter);
    toast->show();
}

void ApplicationUI::addView(View* view) {
    m_pRootPane->add(view);
    m_pViews.append(view);
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    fprintf(stdout, "locale_string %s\n", locale_string.toLocal8Bit().data());
    QString file_name = QString("Char_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
        fprintf(stdout, "setting the label for new file tab");
        // new file tab
        m_pNewFileTab->setTitle(tr("New File"));
        // open file tab
        m_pOpenFileTab->setTitle(tr("Open File"));
        // loop through all the views to reset the texts
        for (int i=0; i<m_pViews.count(); ++i) {
            m_pViews[i]->onLanguageChanged();
        }
    }
}
