/*
 * ShortcutHelp.h
 *
 *  Created on: Jun 9, 2015
 *      Author: lingnan
 */

#ifndef SHORTCUTHELP_H_
#define SHORTCUTHELP_H_

namespace bb {
    namespace cascades {
        class AbstractShortcut;
        class KeyListener;
        class AbstractActionItem;
        class PaneProperties;
    }
}

#define RETURN_SYMBOL QChar(0x23CE)
#define TAB_SYMBOL QChar(0x21E5)
#define SPACE_SYMBOL QChar(0x2423)
#define SHIFT_SYMBOL QChar(0x21E7)
#define BACKSPACE_SYMBOL QChar(0x232B)

class ShortcutHelp
{
public:
    static QList<ShortcutHelp> fromShortcut(bb::cascades::AbstractShortcut *, const QString &prefix=QString());
    static QList<ShortcutHelp> fromKeyListener(bb::cascades::KeyListener *);
    static QList<ShortcutHelp> fromActionItem(bb::cascades::AbstractActionItem *, const QString &prefix=QString());
    static QList<ShortcutHelp> fromPaneProperties(bb::cascades::PaneProperties *, const QString &prefix=QString());
    static QString showAll(const QList<ShortcutHelp> &);
    ShortcutHelp(const QString &_key=QString(), const QString &_help=QString(), const QString &_prefix=QString()) {
        set(_key, _help, _prefix);
    }
    ShortcutHelp(const QChar &_ch, const QString &_help=QString(), const QString &_prefix=QString()) {
        set(QString(_ch), _help, _prefix);
    }
    QString show() const {
        return QString("%1: %2").arg(key, help);
    }
    QString key;
    QString help;
private:
    void set(const QString &_key, const QString &_help, const QString &_prefix) {
        key = _prefix.isNull() ? _key : QString("%1 %2").arg(_prefix, _key);
        help = _help;
    }
};
Q_DECLARE_METATYPE(ShortcutHelp)

#endif /* SHORTCUTHELP_H_ */
