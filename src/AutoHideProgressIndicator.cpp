/*
 * AutoHideProgressIndicator.cpp
 *
 *  Created on: Aug 23, 2015
 *      Author: lingnan
 */

#include <AutoHideProgressIndicator.h>
#include <Utility.h>

AutoHideProgressIndicator::AutoHideProgressIndicator()
{
    setTopMargin(0);
    setVisible(false);
}

void AutoHideProgressIndicator::displayProgress(float progress, bb::cascades::ProgressIndicatorState::Type state)
{
    setState(state);
    setValue(progress);
    setVisible(progress > 0 && progress < 1);
}

void AutoHideProgressIndicator::hide()
{
    setVisible(false);
}
