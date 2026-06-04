#pragma once

#include <QObject>
#include <QString>

namespace QKeychain {
class Job;
class ReadPasswordJob;
}

class CredentialStore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString apiKey READ apiKey NOTIFY apiKeyChanged)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)
    Q_PROPERTY(bool keychainAvailable READ keychainAvailable NOTIFY keychainAvailableChanged)
    Q_PROPERTY(QString storageMode READ storageMode NOTIFY storageModeChanged)

public:
    explicit CredentialStore(QObject *parent = nullptr);

    QString apiKey() const;
    bool loaded() const;
    bool keychainAvailable() const;
    QString storageMode() const;

    Q_INVOKABLE void loadApiKey();
    Q_INVOKABLE void saveApiKey(const QString &key);
    Q_INVOKABLE void clearApiKey();

Q_SIGNALS:
    void apiKeyChanged();
    void loadedChanged();
    void keychainAvailableChanged();
    void storageModeChanged();
    void loadFinished();
    void saveFinished(bool ok, const QString &message);

private:
    QString legacyApiKeyFromSettings() const;
    void removeLegacyApiKeyFromSettings();
    void setApiKey(const QString &key);
    void setLoaded(bool loaded);
    void setKeychainAvailable(bool available);
    void setStorageMode(const QString &mode);
    void readFromKeychain();
    void writeToKeychain(const QString &key, bool migratingLegacy = false);
    void deleteFromKeychain();
    void handleReadFinished(QKeychain::ReadPasswordJob *job);
    void handleWriteFinished(QKeychain::Job *job, bool migratingLegacy);
    void handleDeleteFinished(QKeychain::Job *job);

    QString m_apiKey;
    bool m_loaded = false;
    bool m_keychainAvailable = false;
    QString m_storageMode = QStringLiteral("unavailable");
};
