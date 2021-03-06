/*
 * HighlightStateData.h
 *
 *  Created on: Jan 14, 2015
 *      Author: lingnan
 */

#ifndef HIGHLIGHTSTATEDATA_H_
#define HIGHLIGHTSTATEDATA_H_

#include <boost/shared_ptr.hpp>
#include <srchilite/sourcehighlighter.h>

struct HighlightStateData {
    /// the current state for the SourceHighlighter object
    srchilite::HighlightStatePtr currentState;

    /// the current stack for the SourceHighlighter object
    srchilite::HighlightStateStackPtr stateStack;

    HighlightStateData() {}
    /**
     * Performs a deep copy of the passed object (by duplicating the stack)
     * @param data
     */
    HighlightStateData(const HighlightStateData &data) :
                currentState(data.currentState),
                stateStack(srchilite::HighlightStateStackPtr(
                        new srchilite::HighlightStateStack(*(data.stateStack)))) {
    }

    HighlightStateData(srchilite::HighlightStatePtr currentState_,
            srchilite::HighlightStateStackPtr stateStack_) :
        currentState(currentState_), stateStack(stateStack_) {
    }

    bool operator==(const HighlightStateData &rhs) {
        return (currentState == rhs.currentState || *currentState == *rhs.currentState) &&
                (stateStack == rhs.stateStack || *stateStack == *rhs.stateStack);
    }

    bool operator!=(const HighlightStateData &rhs) {
        return !operator==(rhs);
    }

    typedef boost::shared_ptr<HighlightStateData> ptr;

    static bool equal(ptr lhs, ptr rhs)
    {
        return lhs == rhs || (lhs && rhs && *lhs == *rhs);
    }

    static bool unequal(ptr lhs, ptr rhs)
    {
        return !equal(lhs, rhs);
    }
};




#endif /* HIGHLIGHTSTATEDATA_H_ */
