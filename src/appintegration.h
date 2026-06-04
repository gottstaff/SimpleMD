#pragma once

#include <QObject>
#include <QLocalServer>
#include <QStringList>

class AppIntegration : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList pendingFiles READ pendingFiles NOTIFY pendingFilesChanged)

public:
    explicit AppIntegration(QObject *parent = nullptr);

    QStringList pendingFiles() const;

    static QStringList collectFilePaths(const QStringList &arguments);
    static bool wantsHelp(const QStringList &arguments);
    static void printHelp();

    bool ensurePrimaryInstance(const QString &serverName);
    void setPendingFiles(const QStringList &paths);
    bool sendToRunningInstance(const QString &serverName, const QStringList &paths) const;

Q_SIGNALS:
    void pendingFilesChanged();
    void openFilesRequested(const QStringList &paths);

private:
    void handleIncomingConnection();

    QLocalServer m_server;
    QStringList m_pendingFiles;
};
