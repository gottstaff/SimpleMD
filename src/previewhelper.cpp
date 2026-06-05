#include "previewhelper.h"

#include "imageresolver.h"

#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>

#include <algorithm>

namespace
{
QString preparePreviewMarkdown(const QString &markdown,
                               const QString &documentDirectory,
                               const QString &stagingDirectory,
                               QJsonObject *imagePaths)
{
    if (markdown.isEmpty() || imagePaths == nullptr) {
        return markdown;
    }

    QVector<ImageReference> references = ImageResolver::collectImageReferences(markdown);
    std::sort(references.begin(), references.end(), [](const ImageReference &a, const ImageReference &b) {
        return a.start > b.start;
    });

    int imageId = 0;
    QString updated = markdown;
    for (const ImageReference &ref : references) {
        const QString absolutePath = ImageResolver::resolveReadableImagePath(
            ref.rawRef, documentDirectory, stagingDirectory);
        if (absolutePath.isEmpty()) {
            continue;
        }

        const QString anchor = QStringLiteral("#preview-local-image-") + QString::number(imageId);
        imagePaths->insert(QString::number(imageId), absolutePath);
        updated = ImageResolver::rewriteImageReference(updated, ref, anchor);
        ++imageId;
    }

    return updated;
}
}

PreviewHelper::PreviewHelper(QObject *parent)
    : QObject(parent)
{
}

QString PreviewHelper::buildPreviewScript(const QString &markdown,
                                            const QString &textColor,
                                            const QString &backgroundColor,
                                            const QString &linkColor,
                                            const QString &fontFamily,
                                            int fontSize,
                                            double lineHeight,
                                            int padPx,
                                            int maxWidthEm,
                                            const QString &documentDirectory,
                                            const QString &stagingDirectory,
                                            const QString &scrollbarThumb,
                                            const QString &scrollbarTrack,
                                            const QString &scrollbarThumbHover,
                                            const QString &scrollbarThumbActive) const
{
    QJsonObject theme {
        {QStringLiteral("text"), textColor},
        {QStringLiteral("bg"), backgroundColor},
        {QStringLiteral("link"), linkColor},
        {QStringLiteral("fontFamily"), fontFamily},
        {QStringLiteral("fontSize"), fontSize},
        {QStringLiteral("lineHeight"), lineHeight},
        {QStringLiteral("pad"), padPx},
        {QStringLiteral("maxWidth"), maxWidthEm},
        {QStringLiteral("scrollbarSize"), 3},
    };
    if (!scrollbarThumb.isEmpty()) {
        theme.insert(QStringLiteral("scrollbarThumb"), scrollbarThumb);
    }
    if (!scrollbarTrack.isEmpty()) {
        theme.insert(QStringLiteral("scrollbarTrack"), scrollbarTrack);
    }
    if (!scrollbarThumbHover.isEmpty()) {
        theme.insert(QStringLiteral("scrollbarThumbHover"), scrollbarThumbHover);
    }
    if (!scrollbarThumbActive.isEmpty()) {
        theme.insert(QStringLiteral("scrollbarThumbActive"), scrollbarThumbActive);
    }

    QString documentBaseUrl;
    if (!documentDirectory.isEmpty()) {
        const QUrl baseUrl = QUrl::fromLocalFile(QDir::cleanPath(documentDirectory));
        if (baseUrl.isValid()) {
            documentBaseUrl = baseUrl.toString(QUrl::FullyEncoded);
            if (!documentBaseUrl.endsWith(QLatin1Char('/'))) {
                documentBaseUrl += QLatin1Char('/');
            }
        }
    }

    QJsonObject imagePaths;
    const QString previewMarkdown = preparePreviewMarkdown(
        markdown, documentDirectory, stagingDirectory, &imagePaths);

    const QJsonObject payload {
        {QStringLiteral("markdown"), previewMarkdown},
        {QStringLiteral("theme"), theme},
        {QStringLiteral("documentBaseUrl"), documentBaseUrl},
        {QStringLiteral("imagePaths"), imagePaths},
    };

    const QByteArray json = QJsonDocument(payload).toJson(QJsonDocument::Compact);
    const QString base64 = QString::fromLatin1(json.toBase64());

    return QStringLiteral("updatePreviewFromBase64(\"") + base64 + QStringLiteral("\")");
}
