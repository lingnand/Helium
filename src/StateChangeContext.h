/*
 * StateChangeContext.h
 *
 *  Created on: Apr 25, 2015
 *      Author: lingnan
 */

#ifndef STATECHANGECONTEXT_H_
#define STATECHANGECONTEXT_H_

class View;

// specify the context for a buffer state change
class StateChangeContext
{
public:
    unsigned int requestId;
    View *sourceView;
    bool sourceViewShouldUpdate;
    bool shouldMatchCursorPosition;
    StateChangeContext(unsigned int req=0, View *src=NULL, bool srcChange=true, bool matchCursor=false):
        requestId(req), sourceView(src), sourceViewShouldUpdate(srcChange), shouldMatchCursorPosition(matchCursor) {}
};
Q_DECLARE_METATYPE(StateChangeContext)

#endif /* STATECHANGECONTEXT_H_ */
