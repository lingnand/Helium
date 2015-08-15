/*
 * Defaults.h
 *
 *  Created on: May 23, 2015
 *      Author: lingnan
 */

#ifndef DEFAULTS_H_
#define DEFAULTS_H_

#include <bb/cascades/TextStyle>

namespace bb {
    namespace cascades {
        class Container;
    }
}

class Defaults
{
public:
    static bb::cascades::TextStyle helpText();
    static bb::cascades::TextStyle hintText();
    static bb::cascades::TextStyle centeredBodyText();
    static float space();
};

#endif /* DEFAULTS_H_ */
