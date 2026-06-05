#include "credentialstore.h"

#include <qt6keychain/keychain.h>

#include <QSettings>

namespace
{
constexpr QLatin1StringView kService("io.github.gottstaff.SimpleMD");
constexpr QLatin1StringView kEntryKey("llm-api-key");
constexpr QLatin1StringView kLegacySettingsGroup("Preferences");
constexpr QLatin1StringView kLegacySettingsKey("aiApiKey");
} // namespace

CredentialStore::CredentialStore(QObject *parent)
    : QObject(parent)
{
}

QString CredentialStore::apiKey() const
{
    return m_apiKey;
}

bool CredentialStore::loaded() const
{
    return m_loaded;
}

bool CredentialStore::keychainAvailable() const
{
    return m_keychainAvailable;
}

QString CredentialStore::storageMode() const
{
    return m_storageMode;
}

void CredentialStore::loadApiKey()
{
    readFromKeychain();
}

void CredentialStore::saveApiKey(const QString &key)
{
    const QString trimmed = key.trimmed();
    if (trimmed.isEmpty()) {
        clearApiKey();
        return;
    }

    setApiKey(trimmed);
    writeToKeychain(trimmed);
}

void CredentialStore::clearApiKey()
{
    setApiKey({});
    deleteFromKeychain();
}

QString CredentialStore::legacyApiKeyFromSettings() const
{
    QSettings settings;
    settings.beginGroup(QString::fromLatin1(kLegacySettingsGroup));
    return settings.value(QString::fromLatin1(kLegacySettingsKey)).toString().trimmed();
}

void CredentialStore::removeLegacyApiKeyFromSettings()
{
    QSettings settings;
    settings.beginGroup(QString::fromLatin1(kLegacySettingsGroup));
    settings.remove(QString::fromLatin1(kLegacySettingsKey));
    settings.sync();
}

void CredentialStore::setApiKey(const QString &key)
{
    if (m_apiKey == key) {
        return;
    }
    m_apiKey = key;
    Q_EMIT apiKeyChanged();
}

void CredentialStore::setLoaded(bool loaded)
{
    if (m_loaded == loaded) {
        return;
    }
    m_loaded = loaded;
    Q_EMIT loadedChanged();
}

void CredentialStore::setKeychainAvailable(bool available)
{
    if (m_keychainAvailable == available) {
        return;
    }
    m_keychainAvailable = available;
    Q_EMIT keychainAvailableChanged();
}

void CredentialStore::setStorageMode(const QString &mode)
{
    if (m_storageMode == mode) {
        return;
    }
    m_storageMode = mode;
    Q_EMIT storageModeChanged();
}

void CredentialStore::readFromKeychain()
{
    auto *job = new QKeychain::ReadPasswordJob(QString::fromLatin1(kService), this);
    job->setKey(QString::fromLatin1(kEntryKey));
    job->setAutoDelete(true);
    job->setInsecureFallback(false);
    connect(job, &QKeychain::Job::finished, this, [this](QKeychain::Job *finishedJob) {
        handleReadFinished(static_cast<QKeychain::ReadPasswordJob *>(finishedJob));
    });
    job->start();
}

void CredentialStore::writeToKeychain(const QString &key, bool migratingLegacy)
{
    auto *job = new QKeychain::WritePasswordJob(QString::fromLatin1(kService), this);
    job->setKey(QString::fromLatin1(kEntryKey));
    job->setTextData(key);
    job->setAutoDelete(true);
    job->setInsecureFallback(false);
    connect(job, &QKeychain::Job::finished, this, [this, migratingLegacy](QKeychain::Job *finishedJob) {
        handleWriteFinished(finishedJob, migratingLegacy);
    });
    job->start();
}

void CredentialStore::deleteFromKeychain()
{
    auto *job = new QKeychain::DeletePasswordJob(QString::fromLatin1(kService), this);
    job->setKey(QString::fromLatin1(kEntryKey));
    job->setAutoDelete(true);
    job->setInsecureFallback(false);
    connect(job, &QKeychain::Job::finished, this, [this](QKeychain::Job *finishedJob) {
        handleDeleteFinished(finishedJob);
    });
    job->start();
}

void CredentialStore::handleReadFinished(QKeychain::ReadPasswordJob *job)
{
    const QKeychain::Error error = job->error();
    const QString keychainValue = job->textData().trimmed();

    if (error == QKeychain::NoError && !keychainValue.isEmpty()) {
        setApiKey(keychainValue);
        setKeychainAvailable(true);
        setStorageMode(QStringLiteral("keyring"));
        removeLegacyApiKeyFromSettings();
        setLoaded(true);
        Q_EMIT loadFinished();
        return;
    }

    if (error == QKeychain::NoError || error == QKeychain::EntryNotFound) {
        const QString legacy = legacyApiKeyFromSettings();
        if (!legacy.isEmpty()) {
            setApiKey(legacy);
            writeToKeychain(legacy, true);
            return;
        }

        if (error == QKeychain::EntryNotFound || keychainValue.isEmpty()) {
            if (m_apiKey.isEmpty()) {
                setApiKey({});
                setStorageMode(QStringLiteral("keyring"));
            } else {
                setStorageMode(QStringLiteral("session"));
            }
            setKeychainAvailable(true);
            setLoaded(true);
            Q_EMIT loadFinished();
            return;
        }
    }

    if (error == QKeychain::AccessDenied || error == QKeychain::AccessDeniedByUser) {
        setKeychainAvailable(true);
        setStorageMode(QStringLiteral("locked"));
        setLoaded(true);
        Q_EMIT loadFinished();
        return;
    }

    if (error == QKeychain::NoBackendAvailable) {
        const QString legacy = legacyApiKeyFromSettings();
        if (!legacy.isEmpty() || m_apiKey.isEmpty()) {
            setApiKey(legacy);
        }
        setKeychainAvailable(false);
        setStorageMode(m_apiKey.isEmpty() ? QStringLiteral("unavailable") : QStringLiteral("session"));
        setLoaded(true);
        Q_EMIT loadFinished();
        return;
    }

    const QString legacy = legacyApiKeyFromSettings();
    if (!legacy.isEmpty() || m_apiKey.isEmpty()) {
        setApiKey(legacy);
    }
    setKeychainAvailable(false);
    setStorageMode(m_apiKey.isEmpty() ? QStringLiteral("unavailable") : QStringLiteral("session"));
    setLoaded(true);
    Q_EMIT loadFinished();
}

void CredentialStore::handleWriteFinished(QKeychain::Job *job, bool migratingLegacy)
{
    const QKeychain::Error error = job->error();
    const bool ok = error == QKeychain::NoError;

    if (ok) {
        setKeychainAvailable(true);
        setStorageMode(QStringLiteral("keyring"));
        removeLegacyApiKeyFromSettings();
        if (migratingLegacy) {
            setLoaded(true);
            Q_EMIT loadFinished();
            return;
        }
        Q_EMIT saveFinished(true, {});
        return;
    }

    if (error == QKeychain::NoBackendAvailable) {
        setKeychainAvailable(false);
        setStorageMode(m_apiKey.isEmpty() ? QStringLiteral("unavailable") : QStringLiteral("session"));
    } else {
        setKeychainAvailable(false);
        setStorageMode(m_apiKey.isEmpty() ? QStringLiteral("unavailable") : QStringLiteral("session"));
    }

    if (migratingLegacy) {
        setLoaded(true);
        Q_EMIT loadFinished();
        return;
    }

    Q_EMIT saveFinished(false, job->errorString());
}

void CredentialStore::handleDeleteFinished(QKeychain::Job *job)
{
    const bool ok = job->error() == QKeychain::NoError
        || job->error() == QKeychain::EntryNotFound;

    removeLegacyApiKeyFromSettings();

    if (job->error() == QKeychain::NoError) {
        setKeychainAvailable(true);
        setStorageMode(QStringLiteral("keyring"));
    } else if (job->error() == QKeychain::NoBackendAvailable) {
        setKeychainAvailable(false);
        setStorageMode(QStringLiteral("unavailable"));
    }

    Q_EMIT saveFinished(ok, ok ? QString() : job->errorString());
}
