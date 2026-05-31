#pragma once

#include <QObject>
#include <QPointer>
#include <QQuickWindow>
#include <QColor>
#include <QTimer>

class BracketMatchHighlighter;
class QQuickTextDocument;

class EditorHelper : public QObject
{
    Q_OBJECT

public:
    explicit EditorHelper(QObject *parent = nullptr);

    Q_INVOKABLE void installFileDrop(QQuickWindow *window);
    Q_INVOKABLE void attachBracketMatcher(QObject *textArea, const QColor &highlightColor);
    Q_INVOKABLE void configureSyntaxHighlighting(bool enabled,
                                                 const QColor &heading,
                                                 const QColor &marker,
                                                 const QColor &code,
                                                 const QColor &link,
                                                 const QColor &emphasis);

Q_SIGNALS:
    void filesDropped(const QStringList &paths, qreal x, qreal y);

private Q_SLOTS:
    void refreshBracketMatch();
    void scheduleBracketRefresh();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void updateBracketMatcher(QObject *textArea);

    QPointer<QQuickWindow> m_dropWindow;
    QPointer<QObject> m_textArea;
    BracketMatchHighlighter *m_bracketHighlighter = nullptr;
    bool m_bracketAttachScheduled = false;
    QTimer m_bracketRefreshTimer;

    bool m_syntaxEnabled = false;
    QColor m_syntaxHeading;
    QColor m_syntaxMarker;
    QColor m_syntaxCode;
    QColor m_syntaxLink;
    QColor m_syntaxEmphasis;
    bool m_syntaxColorsConfigured = false;

    void applySyntaxSettings();
};
