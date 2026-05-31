#pragma once

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;

class LlmClient : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString apiBaseUrl READ apiBaseUrl WRITE setApiBaseUrl NOTIFY apiBaseUrlChanged)
    Q_PROPERTY(QString apiKey READ apiKey WRITE setApiKey NOTIFY apiKeyChanged)
    Q_PROPERTY(QString model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QString systemPrompt READ systemPrompt WRITE setSystemPrompt NOTIFY systemPromptChanged)
    Q_PROPERTY(double temperature READ temperature WRITE setTemperature NOTIFY temperatureChanged)
    Q_PROPERTY(int maxTokens READ maxTokens WRITE setMaxTokens NOTIFY maxTokensChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)

public:
    explicit LlmClient(QObject *parent = nullptr);
    ~LlmClient() override;

    QString apiBaseUrl() const;
    void setApiBaseUrl(const QString &value);

    QString apiKey() const;
    void setApiKey(const QString &value);

    QString model() const;
    void setModel(const QString &value);

    QString systemPrompt() const;
    void setSystemPrompt(const QString &value);

    double temperature() const;
    void setTemperature(double value);

    int maxTokens() const;
    void setMaxTokens(int value);

    bool busy() const;
    QString lastError() const;

    Q_INVOKABLE void requestEdit(const QString &instruction,
                                 const QString &selectedText,
                                 const QString &beforeCursor,
                                 const QString &afterCursor);

Q_SIGNALS:
    void completed(const QString &text);
    void failed(const QString &message);
    void apiBaseUrlChanged();
    void apiKeyChanged();
    void modelChanged();
    void systemPromptChanged();
    void temperatureChanged();
    void maxTokensChanged();
    void busyChanged();
    void lastErrorChanged();

private:
    QString endpointUrl() const;
    QString extractContent(const QByteArray &payload, QString *errorMessage) const;
    static QString stripCodeFences(const QString &text);
    void setBusy(bool value);
    void setLastError(const QString &message);

    QNetworkAccessManager *m_network = nullptr;
    QNetworkReply *m_reply = nullptr;

    QString m_apiBaseUrl = QStringLiteral("https://api.openai.com/v1");
    QString m_apiKey;
    QString m_model = QStringLiteral("gpt-4o-mini");
    QString m_systemPrompt = QStringLiteral(
        "You are a precise assistant for editing markdown. Return only the final text to insert. "
        "For Mermaid diagrams use flowchart (not graph), and double-quote labels with parentheses "
        "or special characters, e.g. E[\"Energy density S(f)\"].");
    double m_temperature = 0.4;
    int m_maxTokens = 700;
    bool m_busy = false;
    QString m_lastError;
};
