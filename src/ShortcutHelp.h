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
    static QList<ShortcutHelp> fromShortcut(bb::cascades::AbstractShortcut *);
    static QList<ShortcutHelp> fromKeyListener(bb::cascades::KeyListener *);
    static QList<ShortcutHelp> fromActionItem(bb::cascades::AbstractActionItem *);
    static QList<ShortcutHelp> fromPaneProperties(bb::cascades::PaneProperties *);
    static QString showAll(const QList<ShortcutHelp> &, const QString &keyPrefix=QString());
    ShortcutHelp(const QString &_key=QString(), const QString &_help=QString()):
        key(_key), help(_help) {}
    QString show(const QString &keyPrefix=QString()) const {
        return QString("%1: %2").arg(keyPrefix+key, help);
    }
    QString key;
    QString help;
};
Q_DECLARE_METATYPE(ShortcutHelp)

#endif /* SHORTCUTHELP_H_ */
