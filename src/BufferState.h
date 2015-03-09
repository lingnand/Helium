/*
 * BufferState.h
 *
 *  Created on: Mar 8, 2015
 *      Author: lingnan
 */

#ifndef BUFFERSTATE_H_
#define BUFFERSTATE_H_

#include <QObject>
#include <src/HighlightStateData.h>

struct BufferLine
{
    QString plainText;
    QString highlightText;
    HighlightStateDataPtr beginHighlightState;
    HighlightStateDataPtr endHighlightState;
};
// a buffer state is a snapshot of the current buffer content
// it consists of a list of BufferLinePtr's
// each line is identified by its index inside the BufferState

// on each new highlight iteration, the BufferState is modified
// by comparing the user-changed actual textArea content with the current BufferState,
// referencing the current one as much as possible and add/delete BufferLine's
// when necessary
class BufferState : QList<BufferLine>
{
public:
    // return the index of the line where the cursor is currently in
    // NOTE: the cursorPosition is assumed to be based on plainText
    int indexOfFocus(int cursorPosition);
    QString filetype;
};

#endif /* BUFFERSTATE_H_ */
