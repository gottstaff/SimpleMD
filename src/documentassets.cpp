#include "documentassets.h"

#include "imageresolver.h"

#include <QApplication>
#include <QClipboard>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QImage>
#include <QMimeData>
#include <QStandardPaths>
#include <QUuid>
#include <algorithm>

namespace
{
constexpr QLatin1StringView kStagingDirName(".simplemd-staging");
constexpr QLatin1StringView kImagesDirName("images");
constexpr QLatin1StringView kCacheRoot("simplemd");
}

DocumentAssets::DocumentAssets(QString sessionId)
    : m_sessionId(sessionId.isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces) : std::move(sessionId))
{
}

void DocumentAssets::setDocumentDirectory(const QString &documentDirectory)
{
    m_documentDirectory = documentDirectory.isEmpty() ? QString() : QDir(documentDirectory).absolutePath();
}

QString DocumentAssets::documentDirectory() const
{
    return m_documentDirectory;
}

QString DocumentAssets::stagingDirectory() const
{
    if (!m_documentDirectory.isEmpty()) {
        return QDir(m_documentDirectory).filePath(QString::fromLatin1(kStagingDirName));
    }

    const QString cacheRoot = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (cacheRoot.isEmpty()) {
        return {};
    }

    return QDir(cacheRoot).filePath(QString::fromLatin1(kCacheRoot) + QLatin1Char('/') + m_sessionId);
}

QString DocumentAssets::ensureStagingDirectory() const
{
    const QString staging = stagingDirectory();
    if (staging.isEmpty()) {
        return {};
    }

    QDir dir;
    if (!dir.mkpath(staging)) {
        return {};
    }

    return staging;
}

bool DocumentAssets::clipboardHasImage() const
{
    const QClipboard *clipboard = QApplication::clipboard();
    if (!clipboard) {
        return false;
    }

    const QMimeData *mime = clipboard->mimeData();
    return mime && mime->hasImage();
}

QString DocumentAssets::saveClipboardImage()
{
    const QClipboard *clipboard = QApplication::clipboard();
    if (!clipboard) {
        return {};
    }

    const QMimeData *mime = clipboard->mimeData();
    if (!mime || !mime->hasImage()) {
        return {};
    }

    const QImage image = qvariant_cast<QImage>(mime->imageData());
    if (image.isNull()) {
        return {};
    }

    const QString staging = ensureStagingDirectory();
    if (staging.isEmpty()) {
        return {};
    }

    ++m_pasteCounter;
    const QString fileName = QStringLiteral("pasted-%1.png").arg(m_pasteCounter, 3, 10, QLatin1Char('0'));
    const QString absolutePath = QDir(staging).filePath(fileName);

    if (!image.save(absolutePath, "PNG")) {
        return {};
    }

    return QDir::toNativeSeparators(absolutePath);
}

bool DocumentAssets::isStagedPath(const QString &path) const
{
    if (path.isEmpty()) {
        return false;
    }

    const QString staging = stagingDirectory();
    if (staging.isEmpty()) {
        return false;
    }

    return ImageResolver::isPathUnderDirectory(QFileInfo(path).absoluteFilePath(), staging);
}

QStringList DocumentAssets::listUnstagedAssets(const QString &markdown, const QString &documentDirectory) const
{
    QStringList unstaged;
    const QVector<ImageReference> references = ImageResolver::collectImageReferences(markdown);
    for (const ImageReference &ref : references) {
        if (!ImageResolver::isLocalImageReference(ref.rawRef)) {
            continue;
        }

        const QString absolutePath = ImageResolver::resolveImagePath(ref.rawRef, documentDirectory);
        if (absolutePath.isEmpty()) {
            unstaged.append(ref.rawRef);
            continue;
        }

        if (isStagedPath(absolutePath)) {
            unstaged.append(ref.rawRef);
            continue;
        }

        if (documentDirectory.isEmpty()) {
            unstaged.append(ref.rawRef);
            continue;
        }

        if (!ImageResolver::isPathUnderDirectory(absolutePath, documentDirectory)) {
            unstaged.append(ref.rawRef);
        }
    }

    return unstaged;
}

bool DocumentAssets::isDocumentPackaged(const QString &markdown, const QString &documentDirectory) const
{
    if (documentDirectory.isEmpty()) {
        return false;
    }

    const QVector<ImageReference> references = ImageResolver::collectImageReferences(markdown);
    for (const ImageReference &ref : references) {
        if (!ImageResolver::isLocalImageReference(ref.rawRef)) {
            continue;
        }

        const QFileInfo refInfo(ref.rawRef);
        if (refInfo.isAbsolute()) {
            return false;
        }

        const QString absolutePath = ImageResolver::resolveImagePath(ref.rawRef, documentDirectory);
        if (absolutePath.isEmpty()) {
            return false;
        }

        const QFileInfo fileInfo(absolutePath);
        if (!fileInfo.exists() || !fileInfo.isFile()) {
            return false;
        }

        if (!ImageResolver::isPathUnderDirectory(fileInfo.absoluteFilePath(), documentDirectory)) {
            return false;
        }
    }

    return true;
}

bool DocumentAssets::hasExternalAssets(const QString &markdown, const QString &documentDirectory) const
{
    const QVector<ImageReference> references = ImageResolver::collectImageReferences(markdown);
    for (const ImageReference &ref : references) {
        if (ImageResolver::isExternalOrMissing(ref.rawRef, documentDirectory)) {
            return true;
        }
    }
    return false;
}

QString DocumentAssets::uniqueImageFileName(const QString &sourcePath, const QSet<QString> &usedNames) const
{
    const QFileInfo info(sourcePath);
    QString baseName = info.completeBaseName();
    if (baseName.isEmpty()) {
        baseName = QStringLiteral("image");
    }

    QString suffix = info.suffix().toLower();
    if (suffix.isEmpty()) {
        suffix = QStringLiteral("png");
    }

    QString candidate = baseName + QLatin1Char('.') + suffix;
    int counter = 1;
    while (usedNames.contains(candidate)) {
        candidate = QStringLiteral("%1-%2.%3").arg(baseName).arg(counter).arg(suffix);
        ++counter;
    }

    return candidate;
}

bool DocumentAssets::copyImageToPackage(const QString &sourcePath,
                                        const QString &imagesDirectory,
                                        const QString &targetFileName,
                                        QString *errorMessage) const
{
    const QFileInfo sourceInfo(sourcePath);
    if (!sourceInfo.exists() || !sourceInfo.isFile()) {
        if (errorMessage) {
            *errorMessage = sourcePath;
        }
        return false;
    }

    QDir dir;
    if (!dir.mkpath(imagesDirectory)) {
        if (errorMessage) {
            *errorMessage = imagesDirectory;
        }
        return false;
    }

    const QString destinationPath = QDir(imagesDirectory).filePath(targetFileName);
    if (QFileInfo(destinationPath).absoluteFilePath() == sourceInfo.absoluteFilePath()) {
        return true;
    }

    if (QFile::exists(destinationPath)) {
        QFile::remove(destinationPath);
    }

    if (!QFile::copy(sourceInfo.absoluteFilePath(), destinationPath)) {
        if (errorMessage) {
            *errorMessage = sourceInfo.absoluteFilePath();
        }
        return false;
    }

    return true;
}

QString DocumentAssets::packageRelativePath(const QString &fileName) const
{
    return QString::fromLatin1(kImagesDirName) + QLatin1Char('/') + fileName;
}

void DocumentAssets::removeEmptyStagingDirectory() const
{
    const QString staging = stagingDirectory();
    if (staging.isEmpty()) {
        return;
    }

    QDir dir(staging);
    if (!dir.exists()) {
        return;
    }

    if (dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries).isEmpty()) {
        dir.rmpath(staging);
    }
}

PackageSaveResult DocumentAssets::packageAndSave(const QString &markdown,
                                                 const QString &markdownPath,
                                                 bool packageEnabled) const
{
    PackageSaveResult result;
    result.markdown = markdown;

    const QFileInfo mdInfo(markdownPath);
    const QString docDir = mdInfo.absolutePath();
    if (docDir.isEmpty()) {
        result.errorMessage = QStringLiteral("Invalid save path");
        return result;
    }

    QString updatedMarkdown = markdown;
    int copiedCount = 0;

    if (packageEnabled) {
        const QString imagesDirectory = QDir(docDir).filePath(QString::fromLatin1(kImagesDirName));
        QSet<QString> usedNames;
        QHash<QString, QString> sourceToRelative;

        QVector<ImageReference> references = ImageResolver::collectImageReferences(updatedMarkdown);
        for (const ImageReference &ref : references) {
            if (!ImageResolver::isLocalImageReference(ref.rawRef)) {
                continue;
            }

            const QString sourcePath = ImageResolver::resolveImagePath(ref.rawRef, docDir);
            if (sourcePath.isEmpty()) {
                continue;
            }

            const QFileInfo sourceInfo(sourcePath);
            if (!sourceInfo.exists() || !sourceInfo.isFile()) {
                continue;
            }

            QString relativePath = sourceToRelative.value(sourceInfo.absoluteFilePath());
            if (relativePath.isEmpty()) {
                const QString targetFileName = uniqueImageFileName(sourceInfo.absoluteFilePath(), usedNames);
                QString copyError;
                if (!copyImageToPackage(sourceInfo.absoluteFilePath(), imagesDirectory, targetFileName, &copyError)) {
                    result.errorMessage = copyError;
                    return result;
                }

                usedNames.insert(targetFileName);
                relativePath = packageRelativePath(targetFileName);
                sourceToRelative.insert(sourceInfo.absoluteFilePath(), relativePath);
                ++copiedCount;
            }
        }

        references = ImageResolver::collectImageReferences(updatedMarkdown);
        std::sort(references.begin(), references.end(), [](const ImageReference &a, const ImageReference &b) {
            return a.start > b.start;
        });

        for (const ImageReference &ref : references) {
            if (!ImageResolver::isLocalImageReference(ref.rawRef)) {
                continue;
            }

            const QString sourcePath = ImageResolver::resolveImagePath(ref.rawRef, docDir);
            const QString relativePath = sourceToRelative.value(QFileInfo(sourcePath).absoluteFilePath());
            if (relativePath.isEmpty()) {
                continue;
            }

            updatedMarkdown = ImageResolver::rewriteImageReference(updatedMarkdown, ref, relativePath);
        }
    }

    QFile file(markdownPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        result.errorMessage = file.errorString();
        return result;
    }

    file.write(updatedMarkdown.toUtf8());
    file.close();

    if (packageEnabled) {
        removeEmptyStagingDirectory();
    }

    result.success = true;
    result.markdown = updatedMarkdown;
    result.copiedCount = copiedCount;
    return result;
}
