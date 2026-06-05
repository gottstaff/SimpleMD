#include "imageresolver.h"

#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QUrl>

namespace ImageResolver
{
QString normalizeImageTarget(const QString &reference)
{
    QString target = reference.trimmed();
    if (target.isEmpty()) {
        return target;
    }

    if (target.startsWith(QLatin1Char('<')) && target.endsWith(QLatin1Char('>'))
        && target.size() >= 2) {
        target = target.mid(1, target.size() - 2).trimmed();
    }

    const int doubleQuoteTitle = target.indexOf(QStringLiteral(" \""));
    const int singleQuoteTitle = target.indexOf(QStringLiteral(" '"));
    int titleStart = -1;
    if (doubleQuoteTitle >= 0 && (singleQuoteTitle < 0 || doubleQuoteTitle < singleQuoteTitle)) {
        titleStart = doubleQuoteTitle;
    } else if (singleQuoteTitle >= 0) {
        titleStart = singleQuoteTitle;
    }
    if (titleStart > 0) {
        target = target.left(titleStart).trimmed();
    }

    if (target.contains(QLatin1Char('%'))) {
        const QByteArray decoded = QByteArray::fromPercentEncoding(target.toUtf8());
        if (!decoded.isEmpty()) {
            target = QString::fromUtf8(decoded);
        }
    }

    return target;
}

bool isRemoteOrEmbeddedReference(const QString &reference)
{
    const QString ref = reference.trimmed();
    return ref.isEmpty()
        || ref.startsWith(QLatin1String("data:"), Qt::CaseInsensitive)
        || ref.startsWith(QLatin1String("http://"), Qt::CaseInsensitive)
        || ref.startsWith(QLatin1String("https://"), Qt::CaseInsensitive)
        || ref.startsWith(QLatin1String("qrc:"), Qt::CaseInsensitive)
        || ref.startsWith(QLatin1String("simplemd:"), Qt::CaseInsensitive)
        || ref.startsWith(QLatin1Char('#'));
}

QString resolveImagePath(const QString &reference, const QString &documentDirectory)
{
    QString ref = normalizeImageTarget(reference);
    if (ref.isEmpty() || isRemoteOrEmbeddedReference(ref)) {
        return {};
    }

    if (ref.startsWith(QLatin1String("file://"), Qt::CaseInsensitive)) {
        const QString localPath = QUrl(ref).toLocalFile();
        if (!localPath.isEmpty()) {
            return QFileInfo(localPath).absoluteFilePath();
        }
    }

    QFileInfo info(ref);
    if (info.isAbsolute()) {
        return info.absoluteFilePath();
    }

    if (documentDirectory.isEmpty()) {
        return {};
    }

    return QDir::cleanPath(QFileInfo(QDir(documentDirectory).absoluteFilePath(ref)).absoluteFilePath());
}

QString resolveReadableImagePath(const QString &reference,
                                 const QString &documentDirectory,
                                 const QString &alternateDirectory)
{
    const QString ref = normalizeImageTarget(reference);
    if (ref.isEmpty() || isRemoteOrEmbeddedReference(ref)) {
        return {};
    }

    if (ref.startsWith(QLatin1String("file://"), Qt::CaseInsensitive)) {
        const QString localPath = QUrl(ref).toLocalFile();
        const QFileInfo fileInfo(localPath);
        if (fileInfo.isFile()) {
            return fileInfo.absoluteFilePath();
        }
    }

    const QFileInfo direct(ref);
    if (direct.isAbsolute() && direct.isFile()) {
        return direct.absoluteFilePath();
    }

    const auto firstExistingFile = [&](const QString &baseDirectory) -> QString {
        if (baseDirectory.isEmpty()) {
            return {};
        }

        const QString resolved = resolveImagePath(reference, baseDirectory);
        if (resolved.isEmpty()) {
            return {};
        }

        const QFileInfo info(QDir::cleanPath(resolved));
        return info.isFile() ? info.absoluteFilePath() : QString();
    };

    if (const QString fromDocument = firstExistingFile(documentDirectory); !fromDocument.isEmpty()) {
        return fromDocument;
    }

    if (const QString fromAlternate = firstExistingFile(alternateDirectory); !fromAlternate.isEmpty()) {
        return fromAlternate;
    }

    if (ref.startsWith(QStringLiteral("../"))) {
        QString tail = ref;
        while (tail.startsWith(QStringLiteral("../"))) {
            tail = tail.mid(3);
        }
        if (!tail.isEmpty()) {
            const QString homeRelative = QDir::cleanPath(QDir::homePath() + QLatin1Char('/') + tail);
            const QFileInfo homeInfo(homeRelative);
            if (homeInfo.isFile()) {
                return homeInfo.absoluteFilePath();
            }
        }
    }

    const QString fileName = QFileInfo(ref).fileName();
    if (!fileName.isEmpty()) {
        QStringList downloadRoots;
        const QString xdgDownloads = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
        if (!xdgDownloads.isEmpty()) {
            downloadRoots.append(xdgDownloads);
        }
        downloadRoots.append(QDir::homePath() + QStringLiteral("/Descargas"));
        downloadRoots.append(QDir::homePath() + QStringLiteral("/Downloads"));

        for (const QString &root : downloadRoots) {
            if (root.isEmpty()) {
                continue;
            }
            const QFileInfo candidate(QDir::cleanPath(root + QLatin1Char('/') + fileName));
            if (candidate.isFile()) {
                return candidate.absoluteFilePath();
            }
        }
    }

    return {};
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
            ref.rawRef = normalizeImageTarget(match.captured(2));
            ref.kind = ImageReferenceKind::Markdown;
            references.append(ref);
            pos = match.capturedEnd();
        }
    }

    {
        const QRegularExpression htmlImage(
            QStringLiteral("<img\\b[^>]*\\bsrc\\s*=\\s*(?:\"([^\"]*)\"|'([^']*)'|([^\\s>]+))[^>]*>"),
            QRegularExpression::CaseInsensitiveOption);
        qsizetype pos = 0;
        while (pos < markdown.size()) {
            const QRegularExpressionMatch match = htmlImage.match(markdown, pos);
            if (!match.hasMatch()) {
                break;
            }

            const QString src = !match.captured(1).isEmpty() ? match.captured(1)
                : !match.captured(2).isEmpty() ? match.captured(2)
                                               : match.captured(3);

            ImageReference ref;
            ref.start = int(match.capturedStart());
            ref.length = int(match.capturedLength());
            ref.altOrTag = match.captured(0);
            ref.rawRef = normalizeImageTarget(src);
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
