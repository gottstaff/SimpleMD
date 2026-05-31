#include "previewhelper.h"

#include "imageresolver.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMimeDatabase>
#include <QUrl>

namespace
{
constexpr qint64 kMaxEmbeddedImageBytes = 12 * 1024 * 1024;

QString toDataUrl(const QString &absolutePath)
{
    const QFileInfo info(absolutePath);
    if (!info.exists() || !info.isFile() || info.size() > kMaxEmbeddedImageBytes) {
        return {};
    }

    QFile file(absolutePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }

    const QMimeDatabase mimeDb;
    QString mime = mimeDb.mimeTypeForFile(absolutePath).name();
    if (!mime.startsWith(QLatin1String("image/"))) {
        const QString suffix = info.suffix().toLower();
        if (suffix == QLatin1String("svg")) {
            mime = QStringLiteral("image/svg+xml");
        } else if (suffix == QLatin1String("jpg") || suffix == QLatin1String("jpeg")) {
            mime = QStringLiteral("image/jpeg");
        } else if (suffix == QLatin1String("png")) {
            mime = QStringLiteral("image/png");
        } else if (suffix == QLatin1String("gif")) {
            mime = QStringLiteral("image/gif");
        } else if (suffix == QLatin1String("webp")) {
            mime = QStringLiteral("image/webp");
        } else {
            return {};
        }
    }

    const QByteArray encoded = file.readAll().toBase64();
    if (encoded.isEmpty()) {
        return {};
    }

    return QStringLiteral("data:") + mime + QStringLiteral(";base64,")
        + QString::fromLatin1(encoded);
}

QString embedLocalImages(QString markdown, const QString &documentDirectory)
{
    if (markdown.isEmpty()) {
        return markdown;
    }

    const QVector<ImageReference> references = ImageResolver::collectImageReferences(markdown);
    for (const ImageReference &ref : references) {
        const QString absolutePath = ImageResolver::resolveImagePath(ref.rawRef, documentDirectory);
        const QString dataUrl = absolutePath.isEmpty() ? QString() : toDataUrl(absolutePath);
        if (dataUrl.isEmpty()) {
            continue;
        }

        markdown = ImageResolver::rewriteImageReference(markdown, ref, dataUrl);
    }

    return markdown;
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

    const QString previewMarkdown = embedLocalImages(markdown, documentDirectory);

    const QJsonObject payload {
        {QStringLiteral("markdown"), previewMarkdown},
        {QStringLiteral("theme"), theme},
        {QStringLiteral("documentBaseUrl"), documentBaseUrl},
    };

    const QByteArray json = QJsonDocument(payload).toJson(QJsonDocument::Compact);
    const QString base64 = QString::fromLatin1(json.toBase64());

    return QStringLiteral("updatePreviewFromBase64(\"") + base64 + QStringLiteral("\")");
}
