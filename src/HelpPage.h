/*
 * HelpPage.h
 *
 *  Created on: Jun 7, 2015
 *      Author: lingnan
 */

#ifndef HELPPAGE_H_
#define HELPPAGE_H_

#include <RepushablePage.h>

namespace bb {
    namespace cascades {
        class Option;
        class TextArea;
    }
}
class HelpPage : public RepushablePage
{
    Q_OBJECT
public:
    HelpPage(QObject *parent=NULL);
    Q_SLOT void onTranslatorChanged();
private:
    bb::cascades::Option *_quickStartOption;
    bb::cascades::Option *_keyboardShortcutsOption;
    bb::cascades::Option *_runProfileOption;
    bb::cascades::TextArea *_textArea;
};

#endif /* HELPPAGE_H_ */
