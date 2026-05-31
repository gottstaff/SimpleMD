#pragma once

#include <QSet>
#include <QString>

struct PackageSaveResult {
    bool success = false;
    QString markdown;
    int copiedCount = 0;
    QString errorMessage;
};

class DocumentAssets
{
public:
    explicit DocumentAssets(QString sessionId = {});

    void setDocumentDirectory(const QString &documentDirectory);
    QString documentDirectory() const;

    QString stagingDirectory() const;
    QString ensureStagingDirectory() const;

    bool clipboardHasImage() const;
    QString saveClipboardImage();

    bool isStagedPath(const QString &path) const;
    QStringList listUnstagedAssets(const QString &markdown, const QString &documentDirectory) const;
    bool isDocumentPackaged(const QString &markdown, const QString &documentDirectory) const;
    bool hasExternalAssets(const QString &markdown, const QString &documentDirectory) const;

    PackageSaveResult packageAndSave(const QString &markdown,
                                     const QString &markdownPath,
                                     bool packageEnabled) const;

private:
    QString uniqueImageFileName(const QString &sourcePath, const QSet<QString> &usedNames) const;
    bool copyImageToPackage(const QString &sourcePath,
                            const QString &imagesDirectory,
                            const QString &targetFileName,
                            QString *errorMessage) const;
    QString packageRelativePath(const QString &fileName) const;
    void removeEmptyStagingDirectory() const;

    QString m_sessionId;
    QString m_documentDirectory;
    mutable int m_pasteCounter = 0;
};
