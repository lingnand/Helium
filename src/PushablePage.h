/*
 * PushablePage.h
 *
 *  Created on: May 26, 2015
 *      Author: lingnan
 */

#ifndef PUSHABLEPAGE_H_
#define PUSHABLEPAGE_H_

#include <bb/cascades/Page>

namespace bb {
    namespace cascades {
        class NavigationPane;
    }
}

// a type of page that's intended to be used inside a navigation pane
class PushablePage : public bb::cascades::Page
{
    Q_OBJECT
public:
    PushablePage();
    bb::cascades::NavigationPane *parent() const;
private:
    Q_SLOT void pop();
};

#endif /* PUSHABLEPAGE_H_ */
