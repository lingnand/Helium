/*
 * ModKeyListener.h
 *
 *  Created on: Jan 22, 2015
 *      Author: lingnan
 */

#ifndef MODKEYLISTENER_H_
#define MODKEYLISTENER_H_

#include <bb/cascades/KeyListener>
#include <bb/cascades/TextFieldInputMode>
#include <bb/cascades/TextAreaInputMode>
#include <Utility.h>

namespace bb {
    namespace cascades {
        class KeyEvent;
    }
}

class ModKeyListener : public bb::cascades::KeyListener
{
    Q_OBJECT
public:
    // if sticky is on, modified keys continue to work as long as
    // the mod key is held down
    ModKeyListener(int modKeycap, bool sticky=true);
    virtual ~ModKeyListener() {}
    Q_SLOT void handleFocus(bool focus);
    bool enabled() const;
    Q_SLOT void setEnabled(bool enabled);
    Q_SLOT bool modOff(); // turn off sticky mod

    template <typename BuilderType, typename BuiltType>
    class TBuilder : public BaseObject::TBuilder<BuilderType, BuiltType>
    {
    protected:
          TBuilder(BuiltType* node) : BaseObject::TBuilder<BuilderType, BuiltType>(node)
          {
          }
    public:
          BuilderType& onModifiedKeyReleased(const QObject *receiver, const char *method) {
              this->connect(SIGNAL(modifiedKeyReleased(bb::cascades::KeyEvent*, ModKeyListener*)), receiver, method);
              return this->builder();
          }

          BuilderType& onModifiedKeyPressed(const QObject *receiver, const char *method) {
              this->connect(SIGNAL(modifiedKeyPressed(bb::cascades::KeyEvent*, ModKeyListener*)), receiver, method);
              return this->builder();
          }

          BuilderType& onTextFieldInputModeChanged(const QObject *receiver, const char *method) {
              this->connect(SIGNAL(textFieldInputModeChanged(bb::cascades::TextFieldInputMode::Type)), receiver, method);
              return this->builder();
          }

          BuilderType& onTextAreaInputModeChanged(const QObject* receiver, const char *method) {
              this->connect(SIGNAL(textAreaInputModeChanged(bb::cascades::TextAreaInputMode::Type)), receiver, method);
              return this->builder();
          }

          BuilderType& onModKeyPressedAndReleased(const QObject* receiver, const char *method) {
              this->connect(SIGNAL(modKeyPressedAndReleased(bb::cascades::KeyEvent*, ModKeyListener*)), receiver, method);
              return this->builder();
          }

          BuilderType& modOffOn(const QObject* sender, const char *method) {
              conn(sender, method, &this->instance(), SLOT(modOff()));
              return this->builder();
          }
    };
    class Builder : public TBuilder<Builder, ModKeyListener>
    {
    public:
        explicit Builder(int modKeycap, bool sticky) :
            TBuilder<Builder, ModKeyListener>(new ModKeyListener(modKeycap, sticky)) {}
    };
    static Builder create(int modKeycap, bool sticky=true) {
        return Builder(modKeycap, sticky);
    }
Q_SIGNALS:
    void modifiedKeyPressed(bb::cascades::KeyEvent *event, ModKeyListener *listener);
    // this event won't be generated when sticky is off
    void modifiedKeyReleased(bb::cascades::KeyEvent *event, ModKeyListener *listener);
    // this is when modKey is pressed and released alone, not modifying any key
    void modKeyPressedAndReleased(bb::cascades::KeyEvent *event, ModKeyListener *listener);
    void textFieldInputModeChanged(bb::cascades::TextFieldInputMode::Type newInputMode);
    void textAreaInputModeChanged(bb::cascades::TextAreaInputMode::Type newInputMode);
private:
    int _modKeycap;
    bool _sticky;
    bool _enabled;
    bool _modPressed;
    bool _modUsed;
    bool modOn();
    Q_SLOT void onKeyEvent(bb::cascades::KeyEvent *event);
};

#endif /* MODKEYLISTENER_H_ */
