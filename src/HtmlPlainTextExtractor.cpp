#include <src/HtmlPlainTextExtractor.h>
#include <stdio.h>

bool HtmlPlainTextExtractor::extractPlainText(QTextStream &input, QTextStream &output)
{
    _mode = Extract;
    _output = &output;
    _changed = false;
    parse(input);
    return _changed;
}

bool HtmlPlainTextExtractor::hasPlainText(QTextStream &input)
{
    _mode = Validate;
    _hasPlainText = false;
    parse(input);
    return _hasPlainText;
}

bool HtmlPlainTextExtractor::replacePlainText(QTextStream &input, QTextStream &output, const QList<QPair<TextSelection, QString> > &replaces)
{
    if (replaces.count() == 0)
        return false;

    _mode = Replace;
    _replaces = replaces;
    _output = &output;
    _changed = false;
    parse(input);
    return _changed;
}

bool HtmlPlainTextExtractor::stopParsing()
{
    switch (_mode) {
        case Extract: case Replace:
            return false;
        case Validate:
            return _hasPlainText;
    }
    return false;
}

void HtmlPlainTextExtractor::parseCharacter(const QChar &ch, int charCount)
{
    if (!ch.isNull()) {
        switch (_mode) {
            case Extract:
                (*_output) << ch;
                break;
            case Validate:
                _hasPlainText = true;
                break;
            case Replace: {
                QString replacement = ch;
                if (!_replaces.isEmpty() && charCount > _replaces[0].first.first) {
                    printf("charcount: %d; trying to replace from %d to %d\n",
                            _replaces[0].first.first, _replaces[0].first.second);
                    if (charCount < _replaces[0].first.second) {
                        replacement.clear();
                    } else if (charCount == _replaces[0].first.second) {
                        printf("charcount: %d; trying to replace with %s\n",
                                qPrintable(_replaces[0].second));
                        replacement = _replaces[0].second;
                        _replaces.removeFirst();
                    }
                }
                (*_output) << replacement;
                break;
            }
        }
    }
}

void HtmlPlainTextExtractor::parseTag(const QString &name, const QString &attributeName, const QString &attributeValue)
{
    _changed |= name == "b"
             || name == "i"
             || name == "u"
             || name == "span" && attributeName == "style";
}

void HtmlPlainTextExtractor::parseHtmlCharacter(const QChar &ch)
{
}

void HtmlPlainTextExtractor::reachedEnd()
{
}
