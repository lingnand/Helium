/*
 * ModKeyListener.cpp
 *
 *  Created on: Jan 22, 2015
 *      Author: lingnan
 */

#include <src/ModKeyListener.h>
#include <bb/cascades/KeyEvent>

#define MOD_TIMEOUT 500

ModKeyListener::ModKeyListener(int modKeycap):
    _modKeycap(modKeycap), _modPressed(false), _modUsed(false)
{
    conn(this, SIGNAL(keyEvent(bb::cascades::KeyEvent*)),
        this, SLOT(onKeyEvent(bb::cascades::KeyEvent*)));
}

bool ModKeyListener::modOn()
{
    if (!_modPressed) {
        _modPressed = true;
        emit textFieldInputModeChanged(bb::cascades::TextFieldInputMode::Custom);
        emit textAreaInputModeChanged(bb::cascades::TextAreaInputMode::Custom);
        return true;
    }
    return false;
}

bool ModKeyListener::modOff()
{
    if (_modPressed) {
        emit textFieldInputModeChanged(bb::cascades::TextFieldInputMode::Default);
        emit textAreaInputModeChanged(bb::cascades::TextAreaInputMode::Default);
        _modPressed = false;
        return true;
    }
    return false;
}

void ModKeyListener::handleFocus(bool focus)
{
    if (!focus)
        modOff();
}

void ModKeyListener::onKeyEvent(bb::cascades::KeyEvent *event)
{
    if (event->isPressed()) {
        if (_modPressed) {
            if (event->keycap() == _modKeycap) {
                if (!_modUsed && event->duration() > MOD_TIMEOUT) {
                    _modUsed = true;
                }
            } else {
                _modUsed = true;
                emit modifiedKeyPressed(event);
            }
        } else {
            if (event->keycap() == _modKeycap) {
                modOn();
                _modUsed = false;
            }
        }
    } else {
        if (event->keycap() == _modKeycap) {
            if (modOff() && !_modUsed) {
                emit modKeyPressed(event);
            }
        }
    }
}
