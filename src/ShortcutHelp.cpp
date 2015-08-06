/*
 * ShortcutHelp.cpp
 *
 *  Created on: Jun 9, 2015
 *      Author: lingnan
 */

#include <bb/cascades/Shortcut>
#include <bb/cascades/AbstractActionItem>
#include <bb/cascades/KeyListener>
#include <bb/cascades/NavigationPaneProperties>
#include <ShortcutHelp.h>

using namespace bb::cascades;

QList<ShortcutHelp> ShortcutHelp::fromShortcut(AbstractShortcut *shortcut, const QString &prefix)
{
    QList<ShortcutHelp> list;
    if (shortcut->isEnabled()) {
        if (Shortcut *sh = dynamic_cast<Shortcut *>(shortcut)) {
            QString mod, key = sh->key().toUpper();
            if (key.startsWith("SHIFT+")) {
                mod = QString(SHIFT_SYMBOL) + " ";
                key = key.right(key.size()-6);
            }
            if (key == "BACKSPACE") {
                key = BACKSPACE_SYMBOL;
            } else if (key == "ENTER") {
                key = RETURN_SYMBOL;
            } else if (key == "SPACE") {
                key = SPACE_SYMBOL;
            }
            list.append(ShortcutHelp(mod+key, sh->property("help").toString(), prefix));
        }
    }
    return list;
}

QList<ShortcutHelp> ShortcutHelp::fromKeyListener(KeyListener *listener)
{
    QList<ShortcutHelp> list;
    if (!listener->signalsBlocked()) {
        // get the list of shortcutHelps
        QVariantList vl = listener->property("shortcut_helps").toList();
        for (int i = 0; i < vl.size(); i++) {
            list.append(vl[i].value<ShortcutHelp>());
        }
    }
    return list;
}

QList<ShortcutHelp> ShortcutHelp::fromActionItem(AbstractActionItem *action, const QString &prefix)
{
    QList<ShortcutHelp> list;
    for (int i = 0; i < action->shortcutCount(); i++) {
        action->shortcutAt(i)->setProperty("help", action->title());
        list.append(ShortcutHelp::fromShortcut(action->shortcutAt(i), prefix));
    }
    return list;
}

QList<ShortcutHelp> ShortcutHelp::fromPaneProperties(PaneProperties *properties, const QString &prefix)
{
    QList<ShortcutHelp> list;
    if (NavigationPaneProperties *p = dynamic_cast<NavigationPaneProperties *>(properties)) {
        list.append(ShortcutHelp::fromActionItem((AbstractActionItem *) p->backButton(), prefix));
    }
    return list;

}

QString ShortcutHelp::showAll(const QList<ShortcutHelp> &list)
{
    QString show;
    if (list.empty())
        return show;
    show += list[0].show();
    for (int i = 1; i < list.size(); i++) {
        show += '\n';
        show += list[i].show();
    }
    return show;
}
