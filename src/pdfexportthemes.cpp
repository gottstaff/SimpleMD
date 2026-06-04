#include "pdfexportthemes.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonValue>
#include <QStandardPaths>

namespace
{
constexpr QLatin1StringView kBundledThemesPath(":/pdf-themes.json");
constexpr QLatin1StringView kConfigDirName("SimpleMD");
constexpr QLatin1StringView kConfigFileName("pdf-themes.json");

QStringList themeFieldKeys()
{
    return {
        QStringLiteral("background"),
        QStringLiteral("text"),
        QStringLiteral("link"),
        QStringLiteral("fontFamily"),
        QStringLiteral("codeFontFamily"),
        QStringLiteral("codeBackground"),
        QStringLiteral("titleFontFamily"),
        QStringLiteral("titleFontSize"),
        QStringLiteral("titleColor"),
        QStringLiteral("titleBorderColor"),
        QStringLiteral("headingBorderColor"),
        QStringLiteral("blockquoteBorderColor"),
        QStringLiteral("blockquoteColor"),
        QStringLiteral("tableBorderColor"),
        QStringLiteral("footnotesBorderColor"),
        QStringLiteral("pageNumberColor"),
        QStringLiteral("pageNumberFontFamily"),
        QStringLiteral("pageNumberFontSize"),
    };
}

bool parseThemesDocument(const QJsonDocument &document,
                         QJsonObject &themeDefinitions,
                         QString &defaultThemeId)
{
    if (!document.isObject()) {
        return false;
    }

    const QJsonObject root = document.object();
    const QJsonValue themesValue = root.value(QStringLiteral("themes"));
    if (!themesValue.isObject()) {
        return false;
    }

    themeDefinitions = themesValue.toObject();
    if (themeDefinitions.isEmpty()) {
        return false;
    }

    defaultThemeId = root.value(QStringLiteral("defaultTheme")).toString();
    if (defaultThemeId.isEmpty() || !themeDefinitions.contains(defaultThemeId)) {
        defaultThemeId = themeDefinitions.begin().key();
    }

    return true;
}
} // namespace

PdfExportThemes::PdfExportThemes(QObject *parent)
    : QObject(parent)
{
    reload();
}

QVariantList PdfExportThemes::themes() const
{
    return m_themes;
}

QString PdfExportThemes::defaultThemeId() const
{
    return m_defaultThemeId;
}

QString PdfExportThemes::configFilePath() const
{
    const QString configRoot = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
    if (configRoot.isEmpty()) {
        return {};
    }

    return QDir(configRoot).filePath(QString::fromLatin1(kConfigDirName) + QLatin1Char('/')
                                     + QString::fromLatin1(kConfigFileName));
}

QJsonObject PdfExportThemes::themeData(const QString &themeId) const
{
    const QString resolvedId = themeId.isEmpty() ? m_defaultThemeId : themeId;
    if (m_themeDefinitions.contains(resolvedId)) {
        return mergedTheme(m_themeDefinitions.value(resolvedId).toObject());
    }

    if (m_themeDefinitions.contains(m_defaultThemeId)) {
        return mergedTheme(m_themeDefinitions.value(m_defaultThemeId).toObject());
    }

    return m_fallbackTheme;
}

void PdfExportThemes::reload()
{
    ensureUserConfigFile();

    QJsonObject themeDefinitions;
    QString defaultThemeId;
    const QString userPath = configFilePath();
    if (!userPath.isEmpty() && loadFromFile(userPath)) {
        themeDefinitions = m_themeDefinitions;
        defaultThemeId = m_defaultThemeId;
    } else if (loadBundledDefaults()) {
        themeDefinitions = m_themeDefinitions;
        defaultThemeId = m_defaultThemeId;
    } else {
        themeDefinitions = {};
        defaultThemeId = QStringLiteral("classic");
    }

    m_themeDefinitions = themeDefinitions;
    m_defaultThemeId = defaultThemeId;

    if (m_themeDefinitions.contains(m_defaultThemeId)) {
        m_fallbackTheme = mergedTheme(m_themeDefinitions.value(m_defaultThemeId).toObject());
    } else if (!m_themeDefinitions.isEmpty()) {
        m_fallbackTheme = mergedTheme(m_themeDefinitions.begin().value().toObject());
    } else {
        m_fallbackTheme = QJsonObject {
            {QStringLiteral("background"), QStringLiteral("#ffffff")},
            {QStringLiteral("text"), QStringLiteral("#1a1a1a")},
            {QStringLiteral("link"), QStringLiteral("#2980b9")},
            {QStringLiteral("fontFamily"), QStringLiteral("Georgia, \"Times New Roman\", serif")},
            {QStringLiteral("codeFontFamily"), QStringLiteral("ui-monospace, monospace")},
            {QStringLiteral("codeBackground"), QStringLiteral("rgba(0, 0, 0, 0.06)")},
            {QStringLiteral("titleFontFamily"), QStringLiteral("Georgia, \"Times New Roman\", serif")},
            {QStringLiteral("titleFontSize"), QStringLiteral("8.5pt")},
            {QStringLiteral("titleColor"), QStringLiteral("#888888")},
            {QStringLiteral("titleBorderColor"), QStringLiteral("rgba(0, 0, 0, 0.08)")},
            {QStringLiteral("headingBorderColor"), QStringLiteral("rgba(0, 0, 0, 0.12)")},
            {QStringLiteral("blockquoteBorderColor"), QStringLiteral("rgba(0, 0, 0, 0.18)")},
            {QStringLiteral("blockquoteColor"), QStringLiteral("#333333")},
            {QStringLiteral("tableBorderColor"), QStringLiteral("rgba(0, 0, 0, 0.15)")},
            {QStringLiteral("footnotesBorderColor"), QStringLiteral("rgba(0, 0, 0, 0.12)")},
            {QStringLiteral("pageNumberColor"), QStringLiteral("#666666")},
            {QStringLiteral("pageNumberFontFamily"), QStringLiteral("Georgia, \"Times New Roman\", serif")},
            {QStringLiteral("pageNumberFontSize"), QStringLiteral("9pt")},
        };
    }

    rebuildThemeList();
    Q_EMIT themesChanged();
}

bool PdfExportThemes::loadFromFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QJsonParseError error {};
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning("PdfExportThemes: invalid JSON in %s: %s",
                 qPrintable(path),
                 qPrintable(error.errorString()));
        return false;
    }

    QJsonObject themeDefinitions;
    QString defaultThemeId;
    if (!parseThemesDocument(document, themeDefinitions, defaultThemeId)) {
        qWarning("PdfExportThemes: could not parse themes from %s", qPrintable(path));
        return false;
    }

    m_themeDefinitions = themeDefinitions;
    m_defaultThemeId = defaultThemeId;
    return true;
}

bool PdfExportThemes::loadBundledDefaults()
{
    QFile file(QString::fromLatin1(kBundledThemesPath));
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("PdfExportThemes: bundled themes resource missing");
        return false;
    }

    const QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    QJsonObject themeDefinitions;
    QString defaultThemeId;
    if (!parseThemesDocument(document, themeDefinitions, defaultThemeId)) {
        return false;
    }

    m_themeDefinitions = themeDefinitions;
    m_defaultThemeId = defaultThemeId;
    return true;
}

void PdfExportThemes::ensureUserConfigFile()
{
    const QString userPath = configFilePath();
    if (userPath.isEmpty() || QFile::exists(userPath)) {
        return;
    }

    QFile bundled(QString::fromLatin1(kBundledThemesPath));
    if (!bundled.open(QIODevice::ReadOnly)) {
        return;
    }

    QDir().mkpath(QFileInfo(userPath).absolutePath());

    QFile userFile(userPath);
    if (!userFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning("PdfExportThemes: could not create %s", qPrintable(userPath));
        return;
    }

    userFile.write(bundled.readAll());
}

QJsonObject PdfExportThemes::mergedTheme(const QJsonObject &theme) const
{
    QJsonObject merged = m_fallbackTheme;
    for (const QString &key : themeFieldKeys()) {
        const QJsonValue value = theme.value(key);
        if (value.isString() && !value.toString().isEmpty()) {
            merged.insert(key, value.toString());
        }
    }
    return merged;
}

void PdfExportThemes::rebuildThemeList()
{
    m_themes.clear();

    const QStringList ids = m_themeDefinitions.keys();
    QStringList sortedIds = ids;
    sortedIds.sort();

    for (const QString &id : sortedIds) {
        const QJsonObject theme = m_themeDefinitions.value(id).toObject();
        const QString label = theme.value(QStringLiteral("label")).toString(id);
        m_themes.append(QVariantMap {
            {QStringLiteral("text"), label},
            {QStringLiteral("value"), id},
        });
    }
}
