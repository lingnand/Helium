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
    Q_SLOT void displayProgress(float, bb::cascades::ProgressIndicatorState::Type, const QString &msg);
private:
    Q_SLOT void onProgressMessageDismissed();
};

#endif /* AUTOHIDEPROGRESSINDICATOR_H_ */
