/*
 * FiletypeControl.h
 *
 *  Created on: May 21, 2015
 *      Author: lingnan
 */

#ifndef FILETYPECONTROL_H_
#define FILETYPECONTROL_H_

#include <bb/cascades/Container>

namespace bb {
    namespace cascades {
        class Header;
        class ToggleButton;
        class Label;
    }
}

class Filetype;

class FiletypeControl : public bb::cascades::Container
{
    Q_OBJECT
public:
    FiletypeControl(Filetype *filetype=NULL);
    void setFiletype(Filetype *);
    void reloadFiletypeHeader();
    Q_SLOT void onTranslatorChanged();
private:
    Filetype *_filetype;
    bb::cascades::Header *_highlightHeader;
    bb::cascades::ToggleButton *_highlightToggle;
    bb::cascades::Label *_highlightToggleLabel;
    bb::cascades::Label *_highlightToggleHelp;
    bb::cascades::Header *_runProfileHeader;
};

#endif /* FILETYPECONTROL_H_ */
