/*
 * Defaults.cpp
 *
 *  Created on: May 23, 2015
 *      Author: lingnan
 */

#include <Defaults.h>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/Container>
#include <bb/cascades/Label>
#include <bb/cascades/Color>

using namespace bb::cascades;

TextStyle Defaults::helpText()
{
    TextStyle style = SystemDefaults::TextStyles::bodyText();
    style.setColor(Color::DarkGray);
    return style;
}

TextStyle Defaults::hintText()
{
    TextStyle style = SystemDefaults::TextStyles::bodyText();
    style.setColor(Color::DarkGray);
    style.setFontStyle(FontStyle::Italic);
    return style;
}

TextStyle Defaults::centeredBodyText()
{
    TextStyle style = SystemDefaults::TextStyles::bodyText();
    style.setTextAlign(TextAlign::Center);
    return style;
}

float Defaults::space()
{
    return 20;
}
