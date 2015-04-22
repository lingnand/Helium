/*
 * ModKeyListener.cpp
 *
 *  Created on: Jan 22, 2015
 *      Author: lingnan
 */

#include <bb/cascades/KeyEvent>
#include <ModKeyListener.h>
#include <Utility.h>

#define MOD_TIMEOUT 500

ModKeyListener::ModKeyListener(int modKeycap):
    _modKeycap(modKeycap), _modPressed(false), _modUsed(false), _enabled(true)
{
    conn(this, SIGNAL(keyEvent(bb::cascades::KeyEvent*)),
        this, SLOT(onKeyEvent(bb::cascades::KeyEvent*)));
}

bool ModKeyListener::enabled() const
{
    return _enabled;
}

void ModKeyListener::setEnabled(bool enabled)
{
    _enabled = enabled;
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
    if (!_enabled)
        return;
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
