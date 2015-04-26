#ifndef BUFFERWORKER_H_
#define BUFFERWORKER_H_

#include <QMutex>
#include <bb/cascades/ProgressIndicatorState>
#include <srchilite/sourcehighlight.h>
#include <StateChangeContext.h>
#include <HtmlBufferChangeParser.h>
#include <HighlightStateData.h>
#include <Replacement.h>

namespace srchilite {
    class LangMap;
}

class View;

struct Progress {
    float current;
    float cap;
    Progress(float start=0, float end=1): current(start), cap(end) {}
};
Q_DECLARE_METATYPE(Progress)

class BufferWorker : public QObject
{
    Q_OBJECT
public:
    BufferWorker();
    virtual ~BufferWorker() {}
    const QString &filetype();
    QString filetypeForName(const QString &name);
    Q_SLOT void initialize();
    // all these functions always do the specified job
    Q_SLOT void setFiletype(StateChangeContext &, BufferState &, const QString &filetype, Progress &);
    Q_SLOT BufferStateChange parseBufferChange(BufferState &, const QString &content, ParserPosition, int cursorPosition);
    Q_SLOT void parseAndMergeChange(StateChangeContext &, BufferState &, const QString &content, ParserPosition, int cursorPosition, Progress &);
    Q_SLOT void mergeChange(StateChangeContext &, BufferState &, const BufferStateChange &, Progress &);
    Q_SLOT void replace(StateChangeContext &, BufferState &, const QList<Replacement> &, Progress &);
    Q_SLOT void rehighlight(StateChangeContext &, BufferState &, int index, Progress &);
    Q_SLOT void saveStateToFile(const BufferState &, const QString &filename, Progress &);
    Q_SLOT void loadStateFromFile(StateChangeContext &, const QString &filename, Progress &);
Q_SIGNALS:
    void progressChanged(float progress, bb::cascades::ProgressIndicatorState::Type state=bb::cascades::ProgressIndicatorState::Progress, const QString &msg=QString());
    void filetypeChanged(const StateChangeContext &, const BufferState &);
    void changeMerged(const StateChangeContext &, const BufferState &);
    void occurrenceReplaced(const StateChangeContext &, const BufferState &);
    void stateRehighlighted(const StateChangeContext &, const BufferState &);
    void stateSavedToFile(const QString &filename);
    void stateLoadedFromFile(const StateChangeContext &, const BufferState &, const QString &filename);
private:
    QString _filetype;
    QMutex _langMapMut;
    QMutex _mut;

    HtmlBufferChangeParser _bufferChangeParser;
    HighlightStateData::ptr _mainStateData;
    srchilite::SourceHighlight _sourceHighlight;
    srchilite::LangMap *_langMap;

    QString _filetypeForName(const QString &name);
    void _setFiletype(const QString &filetype);
    void writePlainText(const BufferState &state, QTextStream &output, Progress &);
    BufferStateChange _parseBufferChange(BufferState &state, const QString &content, ParserPosition start, int cursorPosition);
    void _mergeChange(StateChangeContext &, BufferState &, const BufferStateChange &, Progress &progress);
    HighlightStateData::ptr highlightLine(BufferLineState &lineState, HighlightStateData::ptr highlightState);
    void _highlight(BufferState &state, int index,
            HighlightStateData::ptr highlightState, HighlightStateData::ptr oldHighlightState,
            Progress &);
};

QDebug operator<<(QDebug dbg, const BufferLineState *lineState);
QDebug operator<<(QDebug dbg, const Progress &progress);

#endif /* BUFFERWORKER_H_ */
