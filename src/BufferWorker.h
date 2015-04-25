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
    Q_SLOT void setFiletype(StateChangeContext &, BufferState &, const QString &filetype);
    Q_SLOT BufferStateChange parseBufferChange(BufferState &, const QString &content, ParserPosition, int cursorPosition);
    Q_SLOT void parseAndMergeChange(StateChangeContext &, BufferState &, const QString &content, ParserPosition, int cursorPosition, bool trackProgress=true);
    Q_SLOT void mergeChange(StateChangeContext &, BufferState &, const BufferStateChange &, bool trackProgress=true);
    Q_SLOT void replace(StateChangeContext &, BufferState &, const QList<Replacement> &);
    Q_SLOT void rehighlight(StateChangeContext &, BufferState &, int index=0);
    Q_SLOT void saveStateToFile(const BufferState &, const QString &filename);
    Q_SLOT void loadStateFromFile(StateChangeContext &, const QString &filename);
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

    void writePlainText(const BufferState &state, QTextStream &output, float startProgress=0, float endProgress=1);
    BufferStateChange _parseBufferChange(BufferState &state, const QString &content, ParserPosition start, int cursorPosition);
    void _mergeChange(StateChangeContext &, BufferState &, const BufferStateChange &, bool trackProgress=true);
    HighlightStateData::ptr highlightLine(BufferLineState &lineState, HighlightStateData::ptr highlightState);
    void _highlight(BufferState &state, int index,
            HighlightStateData::ptr highlightState, HighlightStateData::ptr oldHighlightState=HighlightStateData::ptr(),
            float startProgress=0, float endProgress=1);
};

#endif /* BUFFERWORKER_H_ */
