#ifndef BUFFERWORKER_H_
#define BUFFERWORKER_H_

#include <QMutex>
#include <src/BufferState.h>
#include <src/HtmlBufferChangeParser.h>
#include <src/srchilite/sourcehighlight.h>
#include <src/HighlightStateData.h>
#include <src/Type.h>

namespace srchilite {
    class LangMap;
}

class View;

class BufferWorker : public QObject
{
    Q_OBJECT
public:
    BufferWorker();
    virtual ~BufferWorker() {}
    const QString &filetype();
    QString filetypeForName(const QString &name);
    Q_SLOT void initialize();
    Q_SLOT void setFiletype(unsigned int requestId, BufferState &state, const QString &filetype);
    Q_SLOT void saveStateToFile(const BufferState &state, const QString &filename);
    Q_SLOT BufferState loadStateFromFile(const QString &filename);
    Q_SLOT void mergeChange(unsigned int requestId, BufferState &state, View *source, const BufferStateChange &change);
    Q_SLOT void replace(unsigned int requestId, BufferState &state, const QList<Replacement> &replaces);
    Q_SLOT void rehighlight(unsigned int requestId, BufferState &state, View *source=NULL, int index=0);
Q_SIGNALS:
    void inProgressChanged(float progress);
    void noUpdate(unsigned int requestId); // didn't trigger a necessary update for Buffer
    void stateLoadedFromFile(const BufferState &state, const QString &filename);
    void stateUpdated(unsigned int requestId, const BufferState &state, View *source=NULL, bool shouldUpdateSourceView=true);
private:
    QString _filetype;
    QMutex _highlightMut;

    HighlightStateData::ptr _mainStateData;
    srchilite::SourceHighlight _sourceHighlight;
    srchilite::LangMap *_langMap;

    HighlightStateData::ptr highlightLine(BufferLineState &lineState, HighlightStateData::ptr highlightState);
    void highlight(BufferState &state, int index, HighlightStateData::ptr highlightState, HighlightStateData::ptr oldHighlightState=HighlightStateData::ptr());
};

#endif /* BUFFERWORKER_H_ */
