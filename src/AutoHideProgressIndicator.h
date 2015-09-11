/*
 * AutoHideProgressIndicator.h
 *
 *  Created on: Aug 23, 2015
 *      Author: lingnan
 */

#ifndef AUTOHIDEPROGRESSINDICATOR_H_
#define AUTOHIDEPROGRESSINDICATOR_H_

#include <bb/cascades/ProgressIndicator>

// mostly used to put at the bottom of a page
class AutoHideProgressIndicator : public bb::cascades::ProgressIndicator
{
    Q_OBJECT
public:
    AutoHideProgressIndicator();
    Q_SLOT void displayProgress(float, bb::cascades::ProgressIndicatorState::Type);
    Q_SLOT void hide();
};

#endif /* AUTOHIDEPROGRESSINDICATOR_H_ */
