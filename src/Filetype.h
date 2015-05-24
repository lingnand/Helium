/*
 * Filetype.h
 *
 *  Created on: May 15, 2015
 *      Author: lingnan
 */

#ifndef FILETYPE_H_
#define FILETYPE_H_

class RunProfileManager;
class Filetype;

struct HighlightType {
    Filetype *filetype;
    bool highlightEnabled;
    HighlightType(Filetype *ft=NULL, bool highlight=false):
        filetype(ft), highlightEnabled(highlight) {}
    bool operator==(const HighlightType &other) const {
        return filetype == other.filetype &&
                highlightEnabled == other.highlightEnabled;
    }
    bool operator!=(const HighlightType &other) const {
        return !operator==(other);
    }
    bool shouldHighlight() const {
        return filetype && highlightEnabled;
    }
};
Q_DECLARE_METATYPE(HighlightType)

class Filetype : public QObject
{
    Q_OBJECT
public:
    Filetype(const QString &name=QString(),
            bool highlightEnabled=true,
            RunProfileManager *runProfileManager=NULL,
            QObject *parent=NULL);
    const QString &name() const { return _name; }
    std::string langName() const; // for working with srchilite
    HighlightType highlightType() { return HighlightType(this, _highlightEnabled); }
    RunProfileManager *runProfileManager() const { return _runProfileManager; }
    void setRunProfileManager(RunProfileManager *);
    bool highlightEnabled() const { return _highlightEnabled; }
    Q_SLOT void setHighlightEnabled(bool);
Q_SIGNALS:
    void runProfileManagerChanged(RunProfileManager *change, RunProfileManager *old);
    void highlightEnabledChanged(bool);
    void highlightTypeChanged(const HighlightType &);
private:
    QString _name;
    bool _highlightEnabled;
    RunProfileManager *_runProfileManager;
};
Q_DECLARE_METATYPE(Filetype *)

QDebug operator<<(QDebug, const Filetype *);

#endif /* FILETYPE_H_ */
