#include "appintegration.h"

#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLocalSocket>
#include <QTextStream>

namespace
{
constexpr int kInstanceConnectMs = 1500;
constexpr int kInstanceWriteMs = 1500;
} // namespace

AppIntegration::AppIntegration(QObject *parent)
    : QObject(parent)
{
    connect(&m_server, &QLocalServer::newConnection, this, &AppIntegration::handleIncomingConnection);
}

QStringList AppIntegration::pendingFiles() const
{
    return m_pendingFiles;
}

bool AppIntegration::wantsHelp(const QStringList &arguments)
{
    for (const QString &arg : arguments) {
        if (arg == QLatin1String("--help") || arg == QLatin1String("-h")) {
            return true;
        }
    }
    return false;
}

void AppIntegration::printHelp()
{
    QTextStream out(stdout);
    out << QStringLiteral("SimpleMD — minimal markdown editor with live preview\n\n")
        << QStringLiteral("Usage: simplemd [OPTIONS] [FILE…]\n\n")
        << QStringLiteral("Options:\n")
        << QStringLiteral("  -h, --help    Show this help and exit\n\n")
        << QStringLiteral("Open one or more markdown files. If SimpleMD is already running,\n")
        << QStringLiteral("files are opened in the existing window.\n");
}

QStringList AppIntegration::collectFilePaths(const QStringList &arguments)
{
    QStringList paths;
    bool endOfOptions = false;

    for (const QString &arg : arguments) {
        if (!endOfOptions && arg == QStringLiteral("--")) {
            endOfOptions = true;
            continue;
        }
        if (!endOfOptions && arg.startsWith(QLatin1Char('-'))) {
            continue;
        }

        const QFileInfo info(arg);
        if (info.isFile()) {
            paths << info.absoluteFilePath();
        }
    }

    return paths;
}

bool AppIntegration::ensurePrimaryInstance(const QString &serverName)
{
    QLocalServer::removeServer(serverName);
    if (!m_server.listen(serverName)) {
        return false;
    }
    return true;
}

void AppIntegration::setPendingFiles(const QStringList &paths)
{
    if (m_pendingFiles == paths) {
        return;
    }
    m_pendingFiles = paths;
    Q_EMIT pendingFilesChanged();
}

bool AppIntegration::sendToRunningInstance(const QString &serverName, const QStringList &paths) const
{
    QLocalSocket socket;
    socket.connectToServer(serverName);
    if (!socket.waitForConnected(kInstanceConnectMs)) {
        return false;
    }

    const QJsonDocument payload(QJsonArray::fromStringList(paths));
    socket.write(payload.toJson(QJsonDocument::Compact));
    socket.flush();
    socket.waitForBytesWritten(kInstanceWriteMs);
    return true;
}

void AppIntegration::handleIncomingConnection()
{
    QLocalSocket *socket = m_server.nextPendingConnection();
    if (!socket) {
        return;
    }

    connect(socket, &QLocalSocket::readyRead, this, [this, socket]() {
        const QByteArray payload = socket->readAll();
        socket->deleteLater();

        const QJsonDocument document = QJsonDocument::fromJson(payload);
        if (!document.isArray()) {
            return;
        }

        QStringList paths;
        for (const QJsonValue &value : document.array()) {
            const QString path = value.toString();
            if (!path.isEmpty()) {
                paths << path;
            }
        }

        if (!paths.isEmpty()) {
            Q_EMIT openFilesRequested(paths);
        } else {
            Q_EMIT openFilesRequested({});
        }
    });
}
