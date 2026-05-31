#include "llmclient.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

LlmClient::LlmClient(QObject *parent)
    : QObject(parent)
    , m_network(new QNetworkAccessManager(this))
{
}

LlmClient::~LlmClient() = default;

QString LlmClient::apiBaseUrl() const
{
    return m_apiBaseUrl;
}

void LlmClient::setApiBaseUrl(const QString &value)
{
    if (m_apiBaseUrl == value) {
        return;
    }
    m_apiBaseUrl = value;
    Q_EMIT apiBaseUrlChanged();
}

QString LlmClient::apiKey() const
{
    return m_apiKey;
}

void LlmClient::setApiKey(const QString &value)
{
    if (m_apiKey == value) {
        return;
    }
    m_apiKey = value;
    Q_EMIT apiKeyChanged();
}

QString LlmClient::model() const
{
    return m_model;
}

void LlmClient::setModel(const QString &value)
{
    if (m_model == value) {
        return;
    }
    m_model = value;
    Q_EMIT modelChanged();
}

QString LlmClient::systemPrompt() const
{
    return m_systemPrompt;
}

void LlmClient::setSystemPrompt(const QString &value)
{
    if (m_systemPrompt == value) {
        return;
    }
    m_systemPrompt = value;
    Q_EMIT systemPromptChanged();
}

double LlmClient::temperature() const
{
    return m_temperature;
}

void LlmClient::setTemperature(double value)
{
    if (qFuzzyCompare(m_temperature, value)) {
        return;
    }
    m_temperature = value;
    Q_EMIT temperatureChanged();
}

int LlmClient::maxTokens() const
{
    return m_maxTokens;
}

void LlmClient::setMaxTokens(int value)
{
    if (m_maxTokens == value) {
        return;
    }
    m_maxTokens = value;
    Q_EMIT maxTokensChanged();
}

bool LlmClient::busy() const
{
    return m_busy;
}

QString LlmClient::lastError() const
{
    return m_lastError;
}

void LlmClient::requestEdit(const QString &instruction,
                            const QString &selectedText,
                            const QString &beforeCursor,
                            const QString &afterCursor)
{
    if (m_busy) {
        setLastError(QStringLiteral("A request is already running."));
        Q_EMIT failed(m_lastError);
        return;
    }
    if (m_apiKey.trimmed().isEmpty()) {
        setLastError(QStringLiteral("Missing API key. Set it in Preferences > AI."));
        Q_EMIT failed(m_lastError);
        return;
    }
    if (m_model.trimmed().isEmpty()) {
        setLastError(QStringLiteral("Missing model name. Set it in Preferences > AI."));
        Q_EMIT failed(m_lastError);
        return;
    }
    if (instruction.trimmed().isEmpty()) {
        setLastError(QStringLiteral("Please enter what you want the model to do."));
        Q_EMIT failed(m_lastError);
        return;
    }

    QJsonObject payload;
    payload[QStringLiteral("model")] = m_model;
    payload[QStringLiteral("temperature")] = m_temperature;
    payload[QStringLiteral("max_tokens")] = m_maxTokens;

    QJsonArray messages;
    messages.push_back(QJsonObject {
        {QStringLiteral("role"), QStringLiteral("system")},
        {QStringLiteral("content"), m_systemPrompt}
    });

    QString userPrompt;
    if (!selectedText.isEmpty()) {
        userPrompt = QStringLiteral(
            "Task:\n%1\n\n"
            "Selected markdown to modify:\n<selected>\n%2\n</selected>\n\n"
            "Return ONLY the rewritten selected text. No explanations.")
                         .arg(instruction, selectedText);
    } else {
        userPrompt = QStringLiteral(
            "Task:\n%1\n\n"
            "Cursor context:\n"
            "<before>\n%2\n</before>\n"
            "<after>\n%3\n</after>\n\n"
            "Return ONLY the text that should be inserted at cursor. No explanations.")
                         .arg(instruction, beforeCursor, afterCursor);
    }

    messages.push_back(QJsonObject {
        {QStringLiteral("role"), QStringLiteral("user")},
        {QStringLiteral("content"), userPrompt}
    });
    payload[QStringLiteral("messages")] = messages;

    QNetworkRequest req { QUrl(endpointUrl()) };
    req.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    req.setRawHeader("Authorization", QByteArray("Bearer ") + m_apiKey.toUtf8());

    m_reply = m_network->post(req, QJsonDocument(payload).toJson(QJsonDocument::Compact));
    setBusy(true);
    setLastError(QString());

    connect(m_reply, &QNetworkReply::finished, this, [this]() {
        if (!m_reply) {
            setBusy(false);
            return;
        }

        const QByteArray responseBytes = m_reply->readAll();
        const QNetworkReply::NetworkError netError = m_reply->error();
        const QString netErrorText = m_reply->errorString();
        m_reply->deleteLater();
        m_reply = nullptr;
        setBusy(false);

        if (netError != QNetworkReply::NoError) {
            QString message = QStringLiteral("Request failed: %1").arg(netErrorText);
            if (!responseBytes.isEmpty()) {
                message += QStringLiteral("\n") + QString::fromUtf8(responseBytes);
            }
            setLastError(message);
            Q_EMIT failed(message);
            return;
        }

        QString parseError;
        const QString content = extractContent(responseBytes, &parseError);
        if (content.isNull()) {
            setLastError(parseError);
            Q_EMIT failed(parseError);
            return;
        }

        Q_EMIT completed(stripCodeFences(content));
    });
}

QString LlmClient::endpointUrl() const
{
    QString base = m_apiBaseUrl.trimmed();
    if (base.endsWith(QLatin1Char('/'))) {
        base.chop(1);
    }
    if (!base.endsWith(QStringLiteral("/chat/completions"))) {
        if (base.endsWith(QStringLiteral("/v1"))) {
            base += QStringLiteral("/chat/completions");
        } else if (base.endsWith(QStringLiteral("/v1/chat"))) {
            base += QStringLiteral("/completions");
        } else {
            base += QStringLiteral("/chat/completions");
        }
    }
    return base;
}

QString LlmClient::extractContent(const QByteArray &payload, QString *errorMessage) const
{
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(payload, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Could not parse model response.");
        }
        return QString();
    }

    const QJsonObject root = doc.object();
    const QJsonArray choices = root.value(QStringLiteral("choices")).toArray();
    if (choices.isEmpty()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Model response has no choices.");
        }
        return QString();
    }

    const QJsonObject first = choices.first().toObject();
    const QJsonObject message = first.value(QStringLiteral("message")).toObject();
    const QJsonValue contentVal = message.value(QStringLiteral("content"));

    if (contentVal.isString()) {
        return contentVal.toString();
    }
    if (contentVal.isArray()) {
        QString combined;
        const QJsonArray parts = contentVal.toArray();
        for (const QJsonValue &part : parts) {
            const QJsonObject obj = part.toObject();
            if (obj.value(QStringLiteral("type")).toString() == QStringLiteral("text")) {
                combined += obj.value(QStringLiteral("text")).toString();
            }
        }
        if (!combined.isEmpty()) {
            return combined;
        }
    }

    if (errorMessage) {
        *errorMessage = QStringLiteral("Model response has no content.");
    }
    return QString();
}

QString LlmClient::stripCodeFences(const QString &text)
{
    QString out = text;
    if (out.startsWith(QStringLiteral("```"))) {
        const int firstNl = out.indexOf(QLatin1Char('\n'));
        if (firstNl >= 0) {
            out = out.mid(firstNl + 1);
        }
        if (out.endsWith(QStringLiteral("```"))) {
            out.chop(3);
        }
    }
    return out.trimmed();
}

void LlmClient::setBusy(bool value)
{
    if (m_busy == value) {
        return;
    }
    m_busy = value;
    Q_EMIT busyChanged();
}

void LlmClient::setLastError(const QString &message)
{
    if (m_lastError == message) {
        return;
    }
    m_lastError = message;
    Q_EMIT lastErrorChanged();
}
