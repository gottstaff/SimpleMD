#pragma once

#include <QColor>
#include <QSyntaxHighlighter>

class BracketMatchHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    enum class DelimiterKind {
        Bracket,
        Backtick,
        Dollar,
    };

    struct Match {
        int start = -1;
        int end = -1;
        int length = 1;
        bool valid = false;
        DelimiterKind kind = DelimiterKind::Bracket;
    };

    explicit BracketMatchHighlighter(QTextDocument *document = nullptr);

    void setCursorPosition(int position);
    void setHighlightColor(const QColor &color);
    void documentContentsChanged();

    void setSyntaxHighlightingEnabled(bool enabled);
    void setSyntaxColors(const QColor &heading,
                         const QColor &marker,
                         const QColor &code,
                         const QColor &link,
                         const QColor &emphasis);

protected:
    void highlightBlock(const QString &text) override;

private:
    enum BlockState {
        NormalState = 0,
        InCodeFenceState = 1,
    };

    static Match findDelimiterMatch(const QString &text, int cursorPos);
    void updateCachedMatch();
    void highlightMarkdownBlock(const QString &text);
    void highlightInlineMarkdown(const QString &text);
    void applyForegroundFormat(int start, int length, const QTextCharFormat &format);

    int m_cursorPos = 0;
    Match m_cachedMatch;
    QTextCharFormat m_format;

    bool m_syntaxEnabled = false;
    QTextCharFormat m_headingFormat;
    QTextCharFormat m_markerFormat;
    QTextCharFormat m_codeFormat;
    QTextCharFormat m_linkFormat;
    QTextCharFormat m_emphasisFormat;
};