#include <src/HtmlPlainTextExtractor.h>
#include <stdio.h>

QString HtmlPlainTextExtractor::extractPlainText(const QString &html)
{
    _mode = Extract;
    _buffer.clear();
    parse(html);
    return _buffer;
}

bool HtmlPlainTextExtractor::hasPlainText(const QString &html)
{
    _mode = Validate;
    _hasPlainText = false;
    parse(html);
    return _hasPlainText;
}

QString HtmlPlainTextExtractor::replacePlainText(const QString &plainText, const QList<QPair<TextSelection, QString> > &replaces)
{
    if (replaces.count() == 0)
        return plainText;

    _mode = Replace;
    _replaces = replaces;
    _buffer.clear();
    parse(plainText);
    return _buffer;
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
                _buffer += ch;
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
                _buffer += replacement;
                break;
            }
        }
    }
}

void HtmlPlainTextExtractor::parseTag(const QString &name, const QString &attributeName, const QString &attributeValue)
{
}

void HtmlPlainTextExtractor::parseHtmlCharacter(const QChar &ch)
{
}

void HtmlPlainTextExtractor::reachedEnd()
{
}
