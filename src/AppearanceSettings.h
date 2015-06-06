/*
 * AppearanceSettings.h
 *
 *  Created on: Jun 6, 2015
 *      Author: lingnan
 */

#ifndef APPEARANCESETTINGS_H_
#define APPEARANCESETTINGS_H_

class AppearanceSettings : public QObject
{
    Q_OBJECT
public:
    AppearanceSettings(bool hideActionBar, QObject *parent=NULL);
    bool hideActionBar() const { return _hideActionBar; }
    Q_SLOT void setHideActionBar(bool);
Q_SIGNALS:
    void hideActionBarChanged(bool);
private:
    bool _hideActionBar;
};

#endif /* APPEARANCESETTINGS_H_ */
