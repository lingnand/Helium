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

#include <QSignalMapper>
#include <bb/cascades/Application>
#include <bb/cascades/TabbedPane>
#include <bb/cascades/Tab>
#include <bb/cascades/Shortcut>
#include <src/View.h>
#include <src/Buffer.h>
#include <src/Utility.h>

using namespace bb::cascades;

#define TAB_TITLE_NEW "New"
#define TAB_TITLE_OPEN "Open"

ApplicationUI::ApplicationUI()
{
    conn(&_localeHandler, SIGNAL(systemLanguageChanged()),
         this, SLOT(onSystemLanguageChanged()));
    conn(this, SIGNAL(translatorChanged()), this, SLOT(onTranslatorChanged()));

    _newViewTab = Tab::create()
        .imageSource(QUrl("asset:///images/ic_compose.png"))
        .addShortcut(Shortcut::create().key("c")
                .onTriggered(this, SLOT(newView())))
        .onTriggered(this, SLOT(newView()));
    _openFileTab = Tab::create()
        .imageSource(QUrl("asset:///images/ic_open.png"))
        .addShortcut(Shortcut::create().key("e")
                .onTriggered(this, SLOT(openFile())))
        .onTriggered(this, SLOT(openFile()));
    // Create the root TabbedPane
    // TODO: handle auto focus properly: focus on the textarea when the tab is switched to
    // However, think about the case when you press enter+<switching between tabs> and you want to do it again on the next switched to tab
    _rootPane = TabbedPane::create()
        .showTabsOnActionBar(false)
        .addShortcut(Shortcut::create().key("q")
                .onTriggered(this, SLOT(prevView())))
        .addShortcut(Shortcut::create().key("w")
                .onTriggered(this, SLOT(nextView())))
        .add(_newViewTab)
        .add(_openFileTab);

    // create a single view
//    appendNewView();
    // create a test case two View one Buffer
    Buffer *buf = new Buffer(100);
    View *v1 = new View(buf), *v2 = new View(buf);
    _rootPane->add(v1);
    _rootPane->add(v2);
    _rootPane->setActiveTab(v1);

    // label initial load
    onSystemLanguageChanged();

    // Set the application scene
    Application::instance()->setScene(_rootPane);
}

void ApplicationUI::newView()
{
    View *c = (View *) _rootPane->activeTab();
    appendNewView();
    c->onOutOfView();
}

void ApplicationUI::appendNewView()
{
    View *v = new View(new Buffer(100));
    conn(this, SIGNAL(translatorChanged()), v, SLOT(onTranslatorChanged()));
    _rootPane->add(v);
    _rootPane->setActiveTab(v);
}

void ApplicationUI::openFile()
{
    Utility::toast("Open file");
}

void ApplicationUI::prevView()
{
    activateViewWithOffset(-1);
}

void ApplicationUI::nextView()
{
    activateViewWithOffset(1);
}

void ApplicationUI::activateViewWithOffset(int offset)
{
    int nv = _rootPane->count() - 2;
    View *c = (View *) _rootPane->activeTab();
    int i = PMOD(_rootPane->indexOf(c) - 2 + offset, nv) + 2;
    _rootPane->setActiveTab(_rootPane->at(i));
    c->onOutOfView();
}

void ApplicationUI::onTranslatorChanged()
{
    _newViewTab->setTitle(tr(TAB_TITLE_NEW));
    _openFileTab->setTitle(tr(TAB_TITLE_OPEN));
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(&_translator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("Char_%1").arg(locale_string);
    if (_translator.load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(&_translator);
        emit translatorChanged();
    }
}
