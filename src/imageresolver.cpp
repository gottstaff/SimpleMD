#include "imageresolver.h"

#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>

namespace ImageResolver
{
bool isRemoteOrEmbeddedReference(const QString &reference)
{
    const QString ref = reference.trimmed();
    return ref.isEmpty()
        || ref.startsWith(QLatin1String("data:"), Qt::CaseInsensitive)
        || ref.startsWith(QLatin1String("http://"), Qt::CaseInsensitive)
        || ref.startsWith(QLatin1String("https://"), Qt::CaseInsensitive)
        || ref.startsWith(QLatin1String("qrc:"), Qt::CaseInsensitive)
        || ref.startsWith(QLatin1Char('#'));
}

QString resolveImagePath(const QString &reference, const QString &documentDirectory)
{
    const QString ref = reference.trimmed();
    if (isRemoteOrEmbeddedReference(ref)) {
        return {};
    }

    const QFileInfo info(ref);
    if (info.isAbsolute()) {
        return info.absoluteFilePath();
    }

    if (documentDirectory.isEmpty()) {
        return {};
    }

    return QDir(documentDirectory).absoluteFilePath(ref);
}

bool isLocalImageReference(const QString &reference)
{
    return !isRemoteOrEmbeddedReference(reference);
}

bool isPathUnderDirectory(const QString &absolutePath, const QString &directory)
{
    if (absolutePath.isEmpty() || directory.isEmpty()) {
        return false;
    }

    const QString cleanDir = QDir::cleanPath(directory);
    const QString cleanPath = QDir::cleanPath(absolutePath);
    if (cleanPath == cleanDir) {
        return true;
    }

    const QString prefix = cleanDir.endsWith(QLatin1Char('/')) ? cleanDir : cleanDir + QLatin1Char('/');
    return cleanPath.startsWith(prefix, Qt::CaseInsensitive);
}

bool isExternalOrMissing(const QString &reference, const QString &documentDirectory)
{
    if (!isLocalImageReference(reference)) {
        return false;
    }

    const QString absolutePath = resolveImagePath(reference, documentDirectory);
    if (absolutePath.isEmpty()) {
        return true;
    }

    const QFileInfo info(absolutePath);
    if (!info.exists() || !info.isFile()) {
        return true;
    }

    if (documentDirectory.isEmpty()) {
        return true;
    }

    return !isPathUnderDirectory(info.absoluteFilePath(), documentDirectory);
}

QVector<ImageReference> collectImageReferences(const QString &markdown)
{
    QVector<ImageReference> references;
    if (markdown.isEmpty()) {
        return references;
    }

    {
        const QRegularExpression mdImage(QStringLiteral(R"(!\[([^\]]*)\]\(([^)]+)\))"));
        qsizetype pos = 0;
        while (pos < markdown.size()) {
            const QRegularExpressionMatch match = mdImage.match(markdown, pos);
            if (!match.hasMatch()) {
                break;
            }

            ImageReference ref;
            ref.start = int(match.capturedStart());
            ref.length = int(match.capturedLength());
            ref.altOrTag = match.captured(1);
            ref.rawRef = match.captured(2).trimmed();
            ref.kind = ImageReferenceKind::Markdown;
            references.append(ref);
            pos = match.capturedEnd();
        }
    }

    {
        const QRegularExpression htmlImage(
            QStringLiteral(R"(<img\b[^>]*\bsrc\s*=\s*(["'])([^"']+)\1[^>]*>)"),
            QRegularExpression::CaseInsensitiveOption);
        qsizetype pos = 0;
        while (pos < markdown.size()) {
            const QRegularExpressionMatch match = htmlImage.match(markdown, pos);
            if (!match.hasMatch()) {
                break;
            }

            ImageReference ref;
            ref.start = int(match.capturedStart());
            ref.length = int(match.capturedLength());
            ref.altOrTag = match.captured(0);
            ref.rawRef = match.captured(2).trimmed();
            ref.kind = ImageReferenceKind::Html;
            references.append(ref);
            pos = match.capturedEnd();
        }
    }

    return references;
}

QString rewriteImageReference(const QString &markdown, const ImageReference &ref, const QString &newRef)
{
    if (ref.start < 0 || ref.length <= 0 || ref.start + ref.length > markdown.size()) {
        return markdown;
    }

    const QString segment = markdown.mid(ref.start, ref.length);
    QString replacement;
    if (ref.kind == ImageReferenceKind::Markdown) {
        replacement = QStringLiteral("![") + ref.altOrTag + QStringLiteral("](") + newRef + QLatin1Char(')');
    } else {
        replacement = ref.altOrTag;
        const QRegularExpression srcAttr(
            QStringLiteral(R"(\bsrc\s*=\s*(["']))") + QRegularExpression::escape(ref.rawRef)
                + QStringLiteral(R"(\1)"),
            QRegularExpression::CaseInsensitiveOption);
        replacement.replace(srcAttr, QStringLiteral("src=\"%1\"").arg(newRef));
    }

    QString updated = markdown;
    updated.replace(ref.start, ref.length, replacement);
    return updated;
}
}
