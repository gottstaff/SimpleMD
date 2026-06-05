#include "previewimagebridge.h"

#include <QFile>
#include <QFileInfo>
#include <QMimeDatabase>

namespace
{
constexpr qint64 kMaxPreviewImageBytes = 16 * 1024 * 1024;
}

PreviewImageBridge::PreviewImageBridge(QObject *parent)
    : QObject(parent)
{
}

QString PreviewImageBridge::loadImageAsDataUrl(const QString &absolutePath) const
{
    const QFileInfo info(QFileInfo(absolutePath).absoluteFilePath());
    if (!info.exists() || !info.isFile() || info.size() > kMaxPreviewImageBytes) {
        return {};
    }

    QFile file(info.absoluteFilePath());
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }

    const QMimeDatabase mimeDb;
    QString mime = mimeDb.mimeTypeForFile(info).name();
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
