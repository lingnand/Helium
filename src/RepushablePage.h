/*
 * RepushablePage.h
 *
 *  Created on: May 26, 2015
 *      Author: lingnan
 */

#ifndef REPUSHABLEPAGE_H_
#define REPUSHABLEPAGE_H_

#include <bb/cascades/Page>

namespace bb {
    namespace cascades {
        class NavigationPane;
    }
}

// a type of page that's reusable across different NavigationPanes
class RepushablePage : public bb::cascades::Page
{
    Q_OBJECT
public:
    RepushablePage(QObject *parent=NULL);
    Q_SLOT void pop();
Q_SIGNALS:
    void toPush(bb::cascades::Page *);
    void toPop();
    void exited();
private:
    QObject *_parent;
};

#endif /* REPUSHABLEPAGE_H_ */
