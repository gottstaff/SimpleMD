#pragma once

#include <QJsonObject>
#include <QObject>
#include <QVariantList>

class PdfExportThemes : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList themes READ themes NOTIFY themesChanged)
    Q_PROPERTY(QString defaultThemeId READ defaultThemeId NOTIFY themesChanged)
    Q_PROPERTY(QString configFilePath READ configFilePath CONSTANT)

public:
    explicit PdfExportThemes(QObject *parent = nullptr);

    QVariantList themes() const;
    QString defaultThemeId() const;
    QString configFilePath() const;

    Q_INVOKABLE QJsonObject themeData(const QString &themeId) const;
    Q_INVOKABLE void reload();

Q_SIGNALS:
    void themesChanged();

private:
    bool loadFromFile(const QString &path);
    bool loadBundledDefaults();
    void ensureUserConfigFile();
    QJsonObject mergedTheme(const QJsonObject &theme) const;
    void rebuildThemeList();

    QVariantList m_themes;
    QString m_defaultThemeId;
    QJsonObject m_themeDefinitions;
    QJsonObject m_fallbackTheme;
};
