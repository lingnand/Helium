/*
 * AppearanceSettings.h
 *
 *  Created on: Jun 6, 2015
 *      Author: lingnan
 */

#ifndef APPEARANCESETTINGS_H_
#define APPEARANCESETTINGS_H_

#include <bb/cascades/VisualStyle>
#include <bb/cascades/FontSize>

class AppearanceSettings : public QObject
{
    Q_OBJECT
public:
    enum Theme { SummerFruit, JellyX, Tomorrow, TomorrowNight };
    AppearanceSettings(
            bool hideActionBar, bool hideTitleBar, bool fullScreen,
            Theme theme, const QString &fontFamily, bb::cascades::FontSize::Type fontSize,
            QObject *parent=NULL):
        QObject(parent),
        _hideActionBar(hideActionBar), _hideTitleBar(hideTitleBar), _fullScreen(fullScreen),
        _theme((Theme) -1), _fontFamily(fontFamily), _fontSize(fontSize) {
        setTheme(theme);
    }
    bool hideActionBar() const { return _hideActionBar; }
    bool shouldHideActionBar() const { return _fullScreen && _hideActionBar; }
    bool hideTitleBar() const { return _hideTitleBar; }
    bool shouldHideTitleBar() const { return _fullScreen && _hideTitleBar; }
    bool fullScreen() const { return _fullScreen; }
    Theme theme() const { return _theme; }
    bb::cascades::VisualStyle::Type visualStyle() const { return _visualStyle; }
    const QString &highlightStyleFile() const { return _highlightStyleFile; }
    Q_SLOT void setHideActionBar(bool h) {
        if (h != _hideActionBar) {
            _hideActionBar = h;
            emit hideActionBarChanged(_hideActionBar);
            if (_fullScreen)
                emit shouldHideActionBarChanged(_hideActionBar);
        }
    }
    Q_SLOT void setHideTitleBar(bool h) {
        if (h != _hideTitleBar) {
            _hideTitleBar = h;
            emit hideTitleBarChanged(_hideTitleBar);
            if (_fullScreen)
                emit shouldHideTitleBarChanged(_hideTitleBar);
        }
    }
    Q_SLOT void setFullScreen(bool f) {
        if (f != _fullScreen) {
            _fullScreen = f;
            emit fullScreenChanged(_fullScreen);
            if (_hideActionBar)
                emit shouldHideActionBarChanged(_fullScreen);
            if (_hideTitleBar)
                emit shouldHideTitleBarChanged(_fullScreen);
        }
    }
    Q_SLOT void setTheme(Theme t) {
        if (t != _theme) {
            _theme = t;
            QString highlightStyle;
            bb::cascades::VisualStyle::Type visualStyle;
            switch (_theme) {
                case SummerFruit:
                    highlightStyle = "summerfruit.style";
                    visualStyle = bb::cascades::VisualStyle::Bright;
                    break;
                case JellyX:
                    highlightStyle = "jellyx.style";
                    visualStyle = bb::cascades::VisualStyle::Dark;
                    break;
                case Tomorrow:
                    highlightStyle = "tomorrow.style";
                    visualStyle = bb::cascades::VisualStyle::Bright;
                    break;
                case TomorrowNight:
                    highlightStyle = "tomorrownight.style";
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
    const QString &fontFamily() const { return _fontFamily; }
    void setFontFamily(const QString &f) {
        if (f != _fontFamily) {
            _fontFamily = f;
            emit fontFamilyChanged(_fontFamily);
        }
    }
    bb::cascades::FontSize::Type fontSize() const { return _fontSize; }
    void setFontSize(bb::cascades::FontSize::Type s) {
        if (s != _fontSize) {
            _fontSize = s;
            emit fontSizeChanged(_fontSize);
        }
    }
Q_SIGNALS:
    void fullScreenChanged(bool);
    void hideActionBarChanged(bool);
    void hideTitleBarChanged(bool);
    void shouldHideActionBarChanged(bool); // fullScreen && hideActionBar
    void shouldHideTitleBarChanged(bool); // fullscreen && hideTitleBar
    void highlightStyleFileChanged(const QString &);
    void visualStyleChanged(bb::cascades::VisualStyle::Type);
    void themeChanged(AppearanceSettings::Theme);
    void fontFamilyChanged(const QString &);
    void fontSizeChanged(bb::cascades::FontSize::Type);
private:
    bool _hideActionBar;
    bool _hideTitleBar;
    bool _fullScreen;
    Theme _theme;
    QString _highlightStyleFile;
    bb::cascades::VisualStyle::Type _visualStyle;
    QString _fontFamily;
    bb::cascades::FontSize::Type _fontSize;
};

#endif /* APPEARANCESETTINGS_H_ */
