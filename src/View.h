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
private:
    bb::cascades::TextField *m_pTitleTextField;
    bb::cascades::TextArea *m_pTextArea;
    Buffer *m_pBuffer;
    Q_SLOT void onTitleTextFieldFocusChanged(bool focus);
    Q_SLOT void onTextAreaTextChanged(const QString& text);
    Q_SLOT void onTextAreaCursorPositionChanged(int position);
    Q_SLOT void setTitle(const QString& title);
    Q_SLOT void setImageSource(const QUrl& image);
};

#endif /* VIEW_H_ */
