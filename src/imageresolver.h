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
bool isRemoteOrEmbeddedReference(const QString &reference);
QString resolveImagePath(const QString &reference, const QString &documentDirectory);
bool isLocalImageReference(const QString &reference);
bool isPathUnderDirectory(const QString &absolutePath, const QString &directory);
bool isExternalOrMissing(const QString &reference, const QString &documentDirectory);
QVector<ImageReference> collectImageReferences(const QString &markdown);
QString rewriteImageReference(const QString &markdown, const ImageReference &ref, const QString &newRef);
}
