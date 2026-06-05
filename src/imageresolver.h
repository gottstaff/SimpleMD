#pragma once

#include <QString>
#include <QVector>

enum class ImageReferenceKind {
    Markdown,
    Html,
};

struct ImageReference {
    int start = -1;
    int length = -1;
    QString rawRef;
    QString altOrTag;
    ImageReferenceKind kind = ImageReferenceKind::Markdown;
};

namespace ImageResolver
{
QString normalizeImageTarget(const QString &reference);
bool isRemoteOrEmbeddedReference(const QString &reference);
QString resolveImagePath(const QString &reference, const QString &documentDirectory);
QString resolveReadableImagePath(const QString &reference,
                                   const QString &documentDirectory,
                                   const QString &alternateDirectory = QString());
bool isLocalImageReference(const QString &reference);
bool isPathUnderDirectory(const QString &absolutePath, const QString &directory);
bool isExternalOrMissing(const QString &reference, const QString &documentDirectory);
QVector<ImageReference> collectImageReferences(const QString &markdown);
QString rewriteImageReference(const QString &markdown, const ImageReference &ref, const QString &newRef);
}
