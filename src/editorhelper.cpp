#include "editorhelper.h"

#include "bracketmatchhighlighter.h"

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QQuickItem>
#include <QQuickTextDocument>
#include <QQuickWindow>
#include <QTextDocument>
#include <QTimer>
#include <QUrl>

EditorHelper::EditorHelper(QObject *parent)
    : QObject(parent)
{
    m_bracketRefreshTimer.setSingleShot(true);
    m_bracketRefreshTimer.setInterval(40);
    connect(&m_bracketRefreshTimer, &QTimer::timeout, this, &EditorHelper::refreshBracketMatch);
}

void EditorHelper::installFileDrop(QQuickWindow *window)
{
    if (m_dropWindow == window) {
        return;
    }

    if (m_dropWindow) {
        m_dropWindow->removeEventFilter(this);
    }

    m_dropWindow = window;
    if (m_dropWindow) {
        m_dropWindow->installEventFilter(this);
        if (QQuickItem *content = m_dropWindow->contentItem()) {
            content->setFlag(QQuickItem::ItemAcceptsDrops, true);
        }
    }
}

void EditorHelper::configureSyntaxHighlighting(bool enabled,
                                               const QColor &heading,
                                               const QColor &marker,
                                               const QColor &code,
                                               const QColor &link,
                                               const QColor &emphasis)
{
    m_syntaxEnabled = enabled;
    m_syntaxHeading = heading;
    m_syntaxMarker = marker;
    m_syntaxCode = code;
    m_syntaxLink = link;
    m_syntaxEmphasis = emphasis;
    m_syntaxColorsConfigured = true;
    applySyntaxSettings();
}

void EditorHelper::applySyntaxSettings()
{
    if (!m_bracketHighlighter || !m_syntaxColorsConfigured) {
        return;
    }

    m_bracketHighlighter->setSyntaxColors(
        m_syntaxHeading, m_syntaxMarker, m_syntaxCode, m_syntaxLink, m_syntaxEmphasis);
    m_bracketHighlighter->setSyntaxHighlightingEnabled(m_syntaxEnabled);
}

void EditorHelper::attachBracketMatcher(QObject *textArea, const QColor &highlightColor)
{
    if (!textArea) {
        return;
    }

    if (m_textArea == textArea && m_bracketHighlighter) {
        m_bracketHighlighter->setHighlightColor(highlightColor);
        applySyntaxSettings();
        updateBracketMatcher(textArea);
        return;
    }

    m_textArea = textArea;

    auto *textDocumentObject = textArea->property("textDocument").value<QObject *>();
    auto *quickDocument = qobject_cast<QQuickTextDocument *>(textDocumentObject);
    if (!quickDocument) {
        return;
    }

    QTextDocument *document = quickDocument->textDocument();
    if (!document) {
        if (!m_bracketAttachScheduled) {
            m_bracketAttachScheduled = true;
            QTimer::singleShot(100, this, [this, textArea, highlightColor]() {
                m_bracketAttachScheduled = false;
                attachBracketMatcher(textArea, highlightColor);
            });
        }
        return;
    }

    if (!m_bracketHighlighter) {
        m_bracketHighlighter = new BracketMatchHighlighter(document);
    } else {
        m_bracketHighlighter->setDocument(document);
    }

    m_bracketHighlighter->setHighlightColor(highlightColor);
    applySyntaxSettings();
    updateBracketMatcher(textArea);

    connect(textArea, SIGNAL(cursorPositionChanged()), this, SLOT(scheduleBracketRefresh()));
    connect(textArea, SIGNAL(textChanged()), this, SLOT(scheduleBracketRefresh()));
    connect(textArea, SIGNAL(selectionStartChanged()), this, SLOT(scheduleBracketRefresh()));
    connect(textArea, SIGNAL(selectionEndChanged()), this, SLOT(scheduleBracketRefresh()));
}

void EditorHelper::scheduleBracketRefresh()
{
    m_bracketRefreshTimer.start();
}

void EditorHelper::refreshBracketMatch()
{
    updateBracketMatcher(m_textArea);
}

void EditorHelper::updateBracketMatcher(QObject *textArea)
{
    if (!m_bracketHighlighter || !textArea) {
        return;
    }

    m_bracketHighlighter->setCursorPosition(textArea->property("cursorPosition").toInt());
}

bool EditorHelper::eventFilter(QObject *watched, QEvent *event)
{
    if (watched != m_dropWindow || !m_dropWindow) {
        return QObject::eventFilter(watched, event);
    }

    switch (event->type()) {
    case QEvent::DragEnter: {
        auto *dragEvent = static_cast<QDragEnterEvent *>(event);
        if (dragEvent->mimeData()->hasUrls()) {
            dragEvent->acceptProposedAction();
            return true;
        }
        break;
    }
    case QEvent::DragMove: {
        auto *dragEvent = static_cast<QDragMoveEvent *>(event);
        if (dragEvent->mimeData()->hasUrls()) {
            dragEvent->acceptProposedAction();
            return true;
        }
        break;
    }
    case QEvent::Drop: {
        auto *dropEvent = static_cast<QDropEvent *>(event);
        if (!dropEvent->mimeData()->hasUrls()) {
            break;
        }

        QStringList paths;
        const QList<QUrl> urls = dropEvent->mimeData()->urls();
        paths.reserve(urls.size());
        for (const QUrl &url : urls) {
            if (url.isLocalFile()) {
                paths.append(url.toLocalFile());
            }
        }

        if (paths.isEmpty()) {
            break;
        }

        dropEvent->acceptProposedAction();
        Q_EMIT filesDropped(paths, dropEvent->position().x(), dropEvent->position().y());
        return true;
    }
    default:
        break;
    }

    return QObject::eventFilter(watched, event);
}
