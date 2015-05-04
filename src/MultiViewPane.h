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

#ifndef MULTIVIEWPANE_H_
#define MULTIVIEWPANE_H_

#include <QList>
#include <bb/cascades/TabbedPane>
#include <Buffer.h>

class Buffer;
class View;

class MultiViewPane : public bb::cascades::TabbedPane
{
    Q_OBJECT
public:
    MultiViewPane(QObject *parent=NULL);
    virtual ~MultiViewPane() {}
    View *activeView() const;
    Q_SLOT void setActiveView(View *, bool toast=false);
    Q_SLOT void setActiveView(int, bool toast=false);
    Q_SLOT void setPrevViewActive(bool toast=true);
    Q_SLOT void setNextViewActive(bool toast=true);
    bb::cascades::Tab *newViewControl() const;
    View *at(int i) const; // the view at a given index
    int activeIndex(int offset=0) const; // the view with the given offset from the active one
    int indexOf(View *) const; // give the index of the given view
    int count() const; // number of views
    void cloneActive(bool toast=true); // clone the current
    void remove(View *, bool toast=true);
    Q_SLOT void addNewView(bool toast=true);
    Buffer *newBuffer();
    Buffer *bufferForFilepath(const QString &filepath);
    void removeBuffer(Buffer *buffer);
    Q_SLOT void onTranslatorChanged();
Q_SIGNALS:
    void translatorChanged();
private:
    // the list of buffers driving the views
    QList<Buffer *> _buffers;
    void insert(int index, View *view);
    void add(View *view);
};

#endif /* MULTIVIEWPANE_H_ */
