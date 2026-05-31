#include "bracketmatchhighlighter.h"

#include <QRegularExpression>
#include <QTextBlock>
#include <QTextDocument>

namespace
{
using Match = BracketMatchHighlighter::Match;

bool isEscaped(const QString &text, int pos)
{
    return pos > 0 && text.at(pos - 1) == u'\\';
}

bool isBracketChar(QChar ch)
{
    return ch == u'(' || ch == u')' || ch == u'[' || ch == u']' || ch == u'{' || ch == u'}';
}

int backtickRunStart(const QString &text, int pos)
{
    pos = qBound(0, pos, text.size() - 1);
    while (pos > 0 && text.at(pos - 1) == u'`') {
        --pos;
    }
    return pos;
}

int backtickRunLength(const QString &text, int index)
{
    int length = 0;
    while (index + length < text.size() && text.at(index + length) == u'`') {
        ++length;
    }
    return length;
}

Match makeMatch(int start, int end, int length, BracketMatchHighlighter::DelimiterKind kind)
{
    Match result;
    result.start = start;
    result.end = end;
    result.length = length;
    result.valid = start >= 0 && end >= start;
    result.kind = kind;
    return result;
}

bool cursorOnChar(int cursorPos, int charIndex)
{
    // QTextEdit cursorPosition is the insertion point before the character at that index.
    return cursorPos == charIndex || cursorPos == charIndex + 1;
}

bool cursorOnDelimiterRun(int cursorPos, int runStart, int delimLen)
{
    for (int i = 0; i < delimLen; ++i) {
        if (cursorOnChar(cursorPos, runStart + i)) {
            return true;
        }
    }
    return false;
}

bool cursorTouchesDelimiterPair(int cursorPos, int open, int closeEnd, int delimLen)
{
    const int closeStart = closeEnd - delimLen + 1;
    return cursorOnDelimiterRun(cursorPos, open, delimLen)
        || cursorOnDelimiterRun(cursorPos, closeStart, delimLen);
}

Match findBracketMatch(const QString &text, int cursorPos)
{
    Match result;

    if (text.isEmpty()) {
        return result;
    }

    int pos = -1;
    if (cursorPos < text.size() && isBracketChar(text.at(cursorPos))) {
        pos = cursorPos;
    } else if (cursorPos > 0 && isBracketChar(text.at(cursorPos - 1))) {
        pos = cursorPos - 1;
    } else {
        return result;
    }

    const QChar ch = text.at(pos);
    const QChar openChar = ch == u'(' || ch == u')' ? u'('
        : ch == u'[' || ch == u']' ? u'['
        : u'{';
    const QChar closeChar = ch == u'(' || ch == u')' ? u')'
        : ch == u'[' || ch == u']' ? u']'
        : u'}';

    if (ch == openChar) {
        int depth = 0;
        for (int i = pos; i < text.size(); ++i) {
            if (text.at(i) == openChar) {
                ++depth;
            } else if (text.at(i) == closeChar) {
                --depth;
                if (depth == 0) {
                    return makeMatch(pos, i, 1, BracketMatchHighlighter::DelimiterKind::Bracket);
                }
            }
        }
        return result;
    }

    int depth = 0;
    for (int i = pos; i >= 0; --i) {
        if (text.at(i) == closeChar) {
            ++depth;
        } else if (text.at(i) == openChar) {
            --depth;
            if (depth == 0) {
                const int start = i;
                depth = 0;
                for (int j = start; j < text.size(); ++j) {
                    if (text.at(j) == openChar) {
                        ++depth;
                    } else if (text.at(j) == closeChar) {
                        --depth;
                        if (depth == 0) {
                            return makeMatch(start, j, 1, BracketMatchHighlighter::DelimiterKind::Bracket);
                        }
                    }
                }
                return result;
            }
        }
    }

    return result;
}

Match findBacktickRunMatch(const QString &text, int cursorPos, int minLength, int maxLength)
{
    Match result;

    if (text.isEmpty() || minLength > maxLength) {
        return result;
    }

    for (int tickLen = maxLength; tickLen >= minLength; --tickLen) {
        const QString tick = QString(tickLen, u'`');
        QList<int> positions;

        for (int i = 0; i <= text.size() - tickLen; ++i) {
            if (text.mid(i, tickLen) != tick || isEscaped(text, i)) {
                continue;
            }

            const QChar before = i > 0 ? text.at(i - 1) : QChar();
            const QChar after = i + tickLen < text.size() ? text.at(i + tickLen) : QChar();
            if (before == u'`' || after == u'`') {
                continue;
            }

            positions.append(i);
        }

        for (int i = 0; i + 1 < positions.size(); i += 2) {
            const int open = positions.at(i);
            const int close = positions.at(i + 1);
            const int closeEnd = close + tickLen - 1;
            if (cursorTouchesDelimiterPair(cursorPos, open, closeEnd, tickLen)) {
                return makeMatch(open, closeEnd, tickLen, BracketMatchHighlighter::DelimiterKind::Backtick);
            }
        }
    }

    return result;
}

Match findBacktickMatch(const QString &text, int cursorPos)
{
    if (text.isEmpty()) {
        return Match{};
    }

    const Match fence = findBacktickRunMatch(text, cursorPos, 3, 32);
    if (fence.valid) {
        return fence;
    }

    if (cursorPos < text.size() && text.at(cursorPos) == u'`') {
        const int start = backtickRunStart(text, cursorPos);
        const int tickLen = backtickRunLength(text, start);
        if (tickLen >= 3) {
            return findBacktickRunMatch(text, cursorPos, tickLen, tickLen);
        }
    } else if (cursorPos > 0 && text.at(cursorPos - 1) == u'`') {
        const int start = backtickRunStart(text, cursorPos - 1);
        const int tickLen = backtickRunLength(text, start);
        if (tickLen >= 3) {
            return findBacktickRunMatch(text, cursorPos, tickLen, tickLen);
        }
    }

    return findBacktickRunMatch(text, cursorPos, 1, 2);
}

Match findDollarDelimited(const QString &text, int cursorPos, int delimLen)
{
    Match result;

    if (text.isEmpty() || delimLen <= 0) {
        return result;
    }

    const QString delim = QString(delimLen, u'$');
    QList<int> positions;

    for (int i = 0; i <= text.size() - delimLen; ++i) {
        if (text.mid(i, delimLen) != delim || isEscaped(text, i)) {
            continue;
        }

        if (delimLen == 1) {
            const QChar before = i > 0 ? text.at(i - 1) : QChar();
            const QChar after = i + 1 < text.size() ? text.at(i + 1) : QChar();
            if (before == u'$' || after == u'$') {
                continue;
            }
        } else if (delimLen == 2) {
            const QChar before = i > 0 ? text.at(i - 1) : QChar();
            const QChar after = i + 2 < text.size() ? text.at(i + 2) : QChar();
            if (before == u'$' || after == u'$') {
                continue;
            }
        }

        positions.append(i);
    }

    for (int i = 0; i + 1 < positions.size(); i += 2) {
        const int open = positions.at(i);
        const int close = positions.at(i + 1);
        const int closeEnd = close + delimLen - 1;
        if (cursorTouchesDelimiterPair(cursorPos, open, closeEnd, delimLen)) {
            return makeMatch(open, closeEnd, delimLen, BracketMatchHighlighter::DelimiterKind::Dollar);
        }
    }

    return result;
}

Match findDollarMatch(const QString &text, int cursorPos)
{
    const Match display = findDollarDelimited(text, cursorPos, 2);
    if (display.valid) {
        return display;
    }
    return findDollarDelimited(text, cursorPos, 1);
}
} // namespace

namespace
{
constexpr int kFullDocumentScanLimit = 50000;
constexpr int kLocalizedScanRadius = 12000;

Match findDelimiterMatchInRange(const QString &text, int cursorPos)
{
    const Match bracket = findBracketMatch(text, cursorPos);
    if (bracket.valid) {
        return bracket;
    }

    const Match backtick = findBacktickMatch(text, cursorPos);
    if (backtick.valid) {
        return backtick;
    }

    return findDollarMatch(text, cursorPos);
}

Match offsetMatch(const Match &match, int offset)
{
    if (!match.valid || offset == 0) {
        return match;
    }

    Match shifted = match;
    shifted.start += offset;
    shifted.end += offset;
    return shifted;
}
} // namespace

BracketMatchHighlighter::BracketMatchHighlighter(QTextDocument *document)
    : QSyntaxHighlighter(document)
{
    m_format.setBackground(QColor(100, 149, 237, 70));
    m_format.setProperty(QTextFormat::FullWidthSelection, false);
}

void BracketMatchHighlighter::setCursorPosition(int position)
{
    m_cursorPos = position;
    updateCachedMatch();
}

void BracketMatchHighlighter::setHighlightColor(const QColor &color)
{
    m_format.setBackground(color);
    updateCachedMatch();
}

void BracketMatchHighlighter::documentContentsChanged()
{
    updateCachedMatch();
}

void BracketMatchHighlighter::setSyntaxHighlightingEnabled(bool enabled)
{
    if (m_syntaxEnabled == enabled) {
        return;
    }
    m_syntaxEnabled = enabled;
    rehighlight();
}

void BracketMatchHighlighter::setSyntaxColors(const QColor &heading,
                                              const QColor &marker,
                                              const QColor &code,
                                              const QColor &link,
                                              const QColor &emphasis)
{
    const auto foreground = [](const QColor &color) {
        QTextCharFormat format;
        format.setForeground(color);
        return format;
    };

    m_headingFormat = foreground(heading);
    m_markerFormat = foreground(marker);
    m_codeFormat = foreground(code);
    m_linkFormat = foreground(link);
    m_emphasisFormat = foreground(emphasis);

    if (m_syntaxEnabled) {
        rehighlight();
    }
}

void BracketMatchHighlighter::applyForegroundFormat(int start, int length, const QTextCharFormat &format)
{
    if (length <= 0 || start < 0) {
        return;
    }
    setFormat(start, length, format);
}

void BracketMatchHighlighter::highlightInlineMarkdown(const QString &text)
{
    static const QRegularExpression inlineCode(QStringLiteral("`[^`\\n]+`"));
    static const QRegularExpression bold(QStringLiteral("\\*\\*[^*\\n]+\\*\\*|__[^_\\n]+__"));
    static const QRegularExpression italic(
        QStringLiteral("(?<!\\*)\\*(?!\\*)[^*\\n]+\\*(?!\\*)|(?<!_)_(?!_)[^_\\n]+_(?!_)"));
    static const QRegularExpression link(QStringLiteral("\\[([^\\]]*)\\]\\(([^)]*)\\)"));

    auto highlightMatches = [&](const QRegularExpression &pattern, const QTextCharFormat &format) {
        QRegularExpressionMatchIterator it = pattern.globalMatch(text);
        while (it.hasNext()) {
            const QRegularExpressionMatch match = it.next();
            applyForegroundFormat(match.capturedStart(), match.capturedLength(), format);
        }
    };

    highlightMatches(inlineCode, m_codeFormat);
    highlightMatches(bold, m_emphasisFormat);
    highlightMatches(italic, m_emphasisFormat);

    QRegularExpressionMatchIterator linkIt = link.globalMatch(text);
    while (linkIt.hasNext()) {
        const QRegularExpressionMatch match = linkIt.next();
        applyForegroundFormat(match.capturedStart(0), match.capturedLength(0), m_linkFormat);
    }
}

void BracketMatchHighlighter::highlightMarkdownBlock(const QString &text)
{
    const bool inFence = previousBlockState() == InCodeFenceState;
    const QString trimmed = text.trimmed();

    if (trimmed.startsWith(QStringLiteral("```"))) {
        applyForegroundFormat(0, text.length(), m_codeFormat);
        setCurrentBlockState(inFence ? NormalState : InCodeFenceState);
        return;
    }

    if (inFence) {
        applyForegroundFormat(0, text.length(), m_codeFormat);
        setCurrentBlockState(InCodeFenceState);
        return;
    }

    setCurrentBlockState(NormalState);

    static const QRegularExpression heading(QStringLiteral("^(#{1,6}\\s+)(.*)$"));
    static const QRegularExpression blockquote(QStringLiteral("^(>\\s)(.*)$"));
    static const QRegularExpression list(QStringLiteral("^(?:\\s*)([-*+]|\\d+\\.)(\\s+)(.*)$"));
    static const QRegularExpression rule(QStringLiteral("^(?:\\s*)([-*_])(?:\\s*\\1){2,}\\s*$"));

    {
        const QRegularExpressionMatch match = heading.match(text);
        if (match.hasMatch()) {
            applyForegroundFormat(match.capturedStart(1), match.capturedLength(1), m_markerFormat);
            applyForegroundFormat(match.capturedStart(2), match.capturedLength(2), m_headingFormat);
            highlightInlineMarkdown(text);
            return;
        }
    }

    {
        const QRegularExpressionMatch quote = blockquote.match(text);
        if (quote.hasMatch()) {
            applyForegroundFormat(quote.capturedStart(1), quote.capturedLength(1), m_markerFormat);
            highlightInlineMarkdown(text);
            return;
        }
    }

    {
        const QRegularExpressionMatch item = list.match(text);
        if (item.hasMatch()) {
            applyForegroundFormat(item.capturedStart(1), item.capturedLength(1), m_markerFormat);
            applyForegroundFormat(item.capturedStart(2), item.capturedLength(2), m_markerFormat);
            highlightInlineMarkdown(text);
            return;
        }
    }

    {
        const QRegularExpressionMatch hr = rule.match(text);
        if (hr.hasMatch()) {
            applyForegroundFormat(0, text.length(), m_markerFormat);
            return;
        }
    }

    highlightInlineMarkdown(text);
}

BracketMatchHighlighter::Match BracketMatchHighlighter::findDelimiterMatch(const QString &text, int cursorPos)
{
    if (text.size() <= kFullDocumentScanLimit) {
        return findDelimiterMatchInRange(text, cursorPos);
    }

    const int start = qMax(0, cursorPos - kLocalizedScanRadius);
    const int end = qMin(text.size(), cursorPos + kLocalizedScanRadius);
    const QString slice = text.mid(start, end - start);
    return offsetMatch(findDelimiterMatchInRange(slice, cursorPos - start), start);
}

void BracketMatchHighlighter::updateCachedMatch()
{
    if (!document()) {
        return;
    }

    const Match previous = m_cachedMatch;
    m_cachedMatch = findDelimiterMatch(document()->toPlainText(), m_cursorPos);
    if (previous.valid == m_cachedMatch.valid
            && previous.start == m_cachedMatch.start
            && previous.end == m_cachedMatch.end
            && previous.length == m_cachedMatch.length) {
        return;
    }
    rehighlight();
}

void BracketMatchHighlighter::highlightBlock(const QString &text)
{
    if (m_syntaxEnabled) {
        highlightMarkdownBlock(text);
    }

    if (!m_cachedMatch.valid) {
        return;
    }

    const QTextBlock block = currentBlock();
    const int blockStart = block.position();
    const int blockLength = block.length() - 1;

    if (m_cachedMatch.start >= blockStart && m_cachedMatch.start < blockStart + blockLength) {
        setFormat(m_cachedMatch.start - blockStart, m_cachedMatch.length, m_format);
    }

    const int closeStart = m_cachedMatch.end - m_cachedMatch.length + 1;
    if (closeStart >= blockStart && closeStart < blockStart + blockLength) {
        setFormat(closeStart - blockStart, m_cachedMatch.length, m_format);
    }
}
