#ifndef BUFFERWORKER_H_
#define BUFFERWORKER_H_

#include <QMutex>
#include <src/BufferState.h>
#include <src/HtmlBufferChangeParser.h>
#include <src/srchilite/sourcehighlight.h>
#include <src/HighlightStateData.h>
#include <src/Type.h>

class View;

class BufferWorker : public QObject
{
    Q_OBJECT
public:
    BufferWorker();
    virtual ~BufferWorker() {}
    const QString &filetype();
    Q_SLOT bool setFiletype(const QString &filetype);
    Q_SLOT void saveStateToFile(const BufferState &state, const QString &filename);
    Q_SLOT BufferState loadStateFromFile(const QString &filename);
    Q_SLOT void mergeChange(BufferState &state, View *source, const BufferStateChange &change);
    Q_SLOT void replace(BufferState &state, const QList<Replacement> &replaces);
    Q_SLOT void highlight(BufferState &state, int index=0);
Q_SIGNALS:
    void inProgressChanged(float progress);
    void stateLoadedFromFile(const BufferState &state, const QString &filename);
    void changeMerged(const BufferState &state, View *source, bool shouldUpdateSourceView);
private:
    QString _filetype;
    QMutex _highlightMut;

    HighlightStateData::ptr _mainStateData;
    srchilite::SourceHighlight _sourceHighlight;

    HighlightStateData::ptr highlightLine(BufferLineState &lineState, HighlightStateData::ptr highlightState);
    void highlight(BufferState &state, int index, HighlightStateData::ptr highlightState, HighlightStateData::ptr oldHighlightState=HighlightStateData::ptr());
};

#endif /* BUFFERWORKER_H_ */
