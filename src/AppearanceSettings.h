/*
 * AppearanceSettings.h
 *
 *  Created on: Jun 6, 2015
 *      Author: lingnan
 */

#ifndef APPEARANCESETTINGS_H_
#define APPEARANCESETTINGS_H_

#include <bb/cascades/VisualStyle>

class AppearanceSettings : public QObject
{
    Q_OBJECT
public:
    enum Theme { Bright, Dark };
    AppearanceSettings(bool hideActionBar, Theme theme, QObject *parent=NULL):
        QObject(parent), _hideActionBar(hideActionBar), _theme((Theme) -1) {
        setTheme(theme);
    }
    bool hideActionBar() const { return _hideActionBar; }
    Theme theme() const { return _theme; }
    bb::cascades::VisualStyle::Type visualStyle() const { return _visualStyle; }
    QString highlightStyleFile() const { return _highlightStyleFile; }
    Q_SLOT void setHideActionBar(bool h) {
        if (h != _hideActionBar) {
            _hideActionBar = h;
            emit hideActionBarChanged(_hideActionBar);
        }
    }
    Q_SLOT void setTheme(Theme t) {
        if (t != _theme) {
            _theme = t;
            QString highlightStyle;
            bb::cascades::VisualStyle::Type visualStyle;
            switch (_theme) {
                case Bright:
                    highlightStyle = "default.style";
                    visualStyle = bb::cascades::VisualStyle::Bright;
                    break;
                case Dark:
                    highlightStyle = "jellyx.style";
                    visualStyle = bb::cascades::VisualStyle::Dark;
                    break;
            }
            if (highlightStyle != _highlightStyleFile) {
                _highlightStyleFile = highlightStyle;
                emit highlightStyleFileChanged(_highlightStyleFile);
            }
            if (visualStyle != _visualStyle) {
                _visualStyle = visualStyle;
                emit visualStyleChanged(_visualStyle);
            }
            emit themeChanged(_theme);
        }
    }
Q_SIGNALS:
    void hideActionBarChanged(bool);
    void highlightStyleFileChanged(const QString &);
    void visualStyleChanged(bb::cascades::VisualStyle::Type);
    void themeChanged(AppearanceSettings::Theme);
private:
    bool _hideActionBar;
    Theme _theme;
    QString _highlightStyleFile;
    bb::cascades::VisualStyle::Type _visualStyle;
};

#endif /* APPEARANCESETTINGS_H_ */
