#ifndef BUFFERWORKER_H_
#define BUFFERWORKER_H_

#include <QMutex>
#include <bb/cascades/ProgressIndicatorState>
#include <srchilite/sourcehighlight.h>
#include <StateChangeContext.h>
#include <HtmlBufferChangeParser.h>
#include <HighlightStateData.h>
#include <Replacement.h>
#include <HighlightType.h>
#include <Progress.h>

class View;

class BufferWorker : public QObject
{
    Q_OBJECT
public:
    BufferWorker();
    virtual ~BufferWorker() {}
    HighlightType highlightType() const;
    // all these functions always do the specified job
    Q_SLOT void setHighlightType(StateChangeContext &, BufferState &, const HighlightType &,
            Progress progress=Progress());
    Q_SLOT BufferStateChange parseBufferChange(BufferState &, const QString &content, ParserPosition, int cursorPosition);
    Q_SLOT void parseAndMergeChange(StateChangeContext &, BufferState &, const QString &content, ParserPosition, int cursorPosition,
            Progress progress=Progress());
    Q_SLOT void mergeChange(StateChangeContext &, BufferState &, const BufferStateChange &,
            Progress progress=Progress());
    Q_SLOT void replace(StateChangeContext &, BufferState &, const QList<Replacement> &,
            Progress progress=Progress());
    Q_SLOT void rehighlight(StateChangeContext &, BufferState &, int index,
            Progress progress=Progress());
    Q_SLOT void saveStateToFile(const BufferState &, const QString &filename,
            Progress progress=Progress());
    Q_SLOT void loadStateFromFile(StateChangeContext &, const QString &filename, bool autodetectFiletype,
            Progress progress=Progress());
Q_SIGNALS:
    void progressChanged(float progress, bb::cascades::ProgressIndicatorState::Type state=bb::cascades::ProgressIndicatorState::Progress, const QString &msg=QString());
    void highlightTypeChanged(const StateChangeContext &, const BufferState &);
    void changeMerged(const StateChangeContext &, const BufferState &);
    void occurrenceReplaced(const StateChangeContext &, const BufferState &);
    void stateRehighlighted(const StateChangeContext &, const BufferState &);
    void stateSavedToFile(const QString &filename);
    void stateLoadedFromFile(const StateChangeContext &, const BufferState &, const QString &filename);
private:
    HighlightType _highlightType;
    QMutex _mut;

    HtmlBufferChangeParser _bufferChangeParser;
    HighlightStateData::ptr _mainStateData;
    srchilite::SourceHighlight _sourceHighlight;

    void _setHighlightType(const HighlightType &highlightType);
    BufferStateChange _parseBufferChange(BufferState &state, const QString &content, ParserPosition start, int cursorPosition);
    void _mergeChange(StateChangeContext &, BufferState &, const BufferStateChange &,
            Progress progress=Progress());
    HighlightStateData::ptr highlightLine(BufferLineState &lineState, HighlightStateData::ptr highlightState);
    void _highlight(BufferState &state, int index,
            HighlightStateData::ptr highlightState, HighlightStateData::ptr oldHighlightState,
            Progress progress=Progress());
};

QDebug operator<<(QDebug dbg, const BufferLineState *lineState);
QDebug operator<<(QDebug dbg, const Progress &progress);

#endif /* BUFFERWORKER_H_ */
