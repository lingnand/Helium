/*
 * View.h
 *
 *  Created on: Nov 21, 2014
 *      Author: lingnan
 */

#ifndef VIEW_H_
#define VIEW_H_

#include <QObject>
#include <bb/cascades/Tab>

namespace bb {
    namespace cascades {
        class TextField;
        class TextArea;
        class AbstractTextControl;
        class ProgressIndicator;
        class KeyEvent;
        class ActionItem;
    }
}

class Buffer;

class View : public bb::cascades::Tab
{
    Q_OBJECT
public:
    View(Buffer* buffer);
    virtual ~View() {}
    Q_SLOT void onLanguageChanged();
    Q_SLOT void setBuffer(Buffer* buffer);
    Q_SLOT void lockTextArea();
    Q_SLOT void unlockTextArea();
private:
    bb::cascades::TextField *_titleTextField;
    bb::cascades::TextArea *_textArea;
    bb::cascades::ProgressIndicator *_progressIndicator;
    bb::cascades::ActionItem *_saveAction;
    Buffer *_buffer;
    unsigned char _modPressed;
    Q_SLOT void onTitleTextFieldFocusChanged(bool focus);
    Q_SLOT void onTitleTextFieldSubmitted();
    Q_SLOT void onTextAreaTextChanged(const QString& text);
    Q_SLOT void onTextAreaKeyEvent(bb::cascades::KeyEvent *event);
    Q_SLOT void onBufferFiletypeChanged(const QString& filetype);
    Q_SLOT void onBufferContentChanged(const QString& content);
    Q_SLOT void onBufferProgressChanged(float progress);
    Q_SLOT void autoFocus();
    Q_SLOT void setTitle(const QString& title);
};

#endif /* VIEW_H_ */
