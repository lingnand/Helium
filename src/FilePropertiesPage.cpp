/*
 * FilePropertiesPage.cpp
 *
 *  Created on: May 23, 2015
 *      Author: lingnan
 */

#include <bb/cascades/TitleBar>
#include <bb/cascades/ToggleButton>
#include <bb/cascades/Label>
#include <bb/cascades/DropDown>
#include <bb/cascades/ActionItem>
#include <bb/cascades/Divider>
#include <bb/cascades/NavigationPaneProperties>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/SystemDefaults>
#include <bb/cascades/Shortcut>
#include <FilePropertiesPage.h>
#include <Filetype.h>
#include <FiletypeMap.h>
#include <FiletypeControl.h>
#include <Segment.h>
#include <Helium.h>
#include <Utility.h>
#include <SignalBlocker.h>

using namespace bb::cascades;

FilePropertiesPage::FilePropertiesPage():
    _autodetectFiletypeToggleLabel(Label::create()
        .layoutProperties(StackLayoutProperties::create().spaceQuota(1))
        .textStyle(SystemDefaults::TextStyles::primaryText())),
    _autodetectFiletypeToggle(ToggleButton::create()
        .checked(true)),
    _autodetectFiletypeToggleHelp(Label::create()
        .textStyle(Defaults::helpText())),
    _filetypeSelect(DropDown::create()
        .add("", QVariant::fromValue((Filetype *) NULL))
        .enabled(false)),
    _filetypeSelectHelp(Label::create()
        .multiline(true)
        .textStyle(Defaults::helpText())),
    _filetypeControl(new FiletypeControl),
    _titleBar(TitleBar::create()),
    _backButton(ActionItem::create()
        .addShortcut(Shortcut::create().key("x"))
        .onTriggered(this, SIGNAL(backButtonTriggered())))
{
    _autodetectFiletypeToggle->setChecked(true);
    conn(_autodetectFiletypeToggle, SIGNAL(checkedChanged(bool)),
        this, SIGNAL(autodetectFiletypeCheckedChanged(bool)));

    QList<Filetype *> filetypes = Helium::instance()->filetypeMap()->filetypes();
    for (int i = 0; i < filetypes.size(); i++) {
        Filetype *f = filetypes[i];
        _filetypeSelect->add(Option::create()
            .text(f->name())
            .value(QVariant::fromValue(f)));
    }
    conn(_filetypeSelect, SIGNAL(selectedValueChanged(const QVariant)),
        this, SLOT(onFiletypeSelectionChanged(const QVariant)));

    setTitleBar(_titleBar);
    setContent(Segment::create().section()
        .add(Segment::create().subsection().leftToRight()
            .add(_autodetectFiletypeToggleLabel)
            .add(_autodetectFiletypeToggle))
        .add(Segment::create().subsection().add(_autodetectFiletypeToggleHelp))
        .add(Divider::create())
        .add(Segment::create().subsection().add(_filetypeSelectHelp))
        .add(Segment::create().subsection().add(_filetypeSelect))
        .add(Divider::create()));
    setPaneProperties(NavigationPaneProperties::create()
        .backButton(_backButton));
    onTranslatorChanged();
}

void FilePropertiesPage::setAutodetectFiletypeChecked(bool checked)
{
    _autodetectFiletypeToggle->setChecked(checked);
    _filetypeSelect->setEnabled(!checked);
}

Segment *FilePropertiesPage::content() const
{
    return (Segment *) bb::cascades::Page::content();
}

void FilePropertiesPage::setFiletype(Filetype *filetype)
{
    if (filetype) {
        for (int i = 0; i < _filetypeSelect->count(); i++) {
            if (_filetypeSelect->at(i)->value().value<Filetype *>() == filetype) {
                SignalBlocker block(_filetypeSelect);
                _filetypeSelect->setSelectedIndex(i);
            }
        }
        content()->add(_filetypeControl);
    } else {
        _filetypeSelect->setSelectedIndex(0);
        content()->remove(_filetypeControl);
    }
    _filetypeControl->setFiletype(filetype);
}

void FilePropertiesPage::onFiletypeSelectionChanged(const QVariant v)
{
    emit filetypeSelectionChanged(v.value<Filetype *>());
}

void FilePropertiesPage::onTranslatorChanged()
{
    _titleBar->setTitle(tr("File Properties"));
    _autodetectFiletypeToggleLabel->setText(tr("Autodetect filetype"));
    _autodetectFiletypeToggleHelp->setText(tr("Automatically change filetype based on filename"));
    _filetypeSelectHelp->setText(tr("The filetype for the current buffer. Each filetype uses a different set of highlight rules and comes with separate settings."));
    _filetypeSelect->setTitle(tr("Filetype"));
    _backButton->setTitle(tr("Back"));
    _filetypeControl->onTranslatorChanged();
}
