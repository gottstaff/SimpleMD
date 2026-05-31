#pragma once

#include "documentassets.h"

#include <QObject>
#include <QStringList>
#include <QTimer>

class DocumentController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString filePath READ filePath NOTIFY filePathChanged)
    Q_PROPERTY(bool modified READ modified NOTIFY modifiedChanged)
    Q_PROPERTY(QString windowTitle READ windowTitle NOTIFY windowTitleChanged)
    Q_PROPERTY(QStringList recentFiles READ recentFiles NOTIFY recentFilesChanged)
    Q_PROPERTY(bool prefersPackaging READ prefersPackaging NOTIFY prefersPackagingChanged)
    Q_PROPERTY(int unstagedAssetCount READ unstagedAssetCount NOTIFY unstagedAssetCountChanged)

public:
    explicit DocumentController(QObject *parent = nullptr);

    QString text() const;
    void setText(const QString &text);

    QString filePath() const;
    bool modified() const;
    QString windowTitle() const;
    QStringList recentFiles() const;
    bool prefersPackaging() const;
    int unstagedAssetCount() const;

    Q_INVOKABLE bool newDocument();
    Q_INVOKABLE bool open();
    Q_INVOKABLE bool openFile(const QString &path);
    Q_INVOKABLE bool openRecent(const QString &path);
    Q_INVOKABLE bool save();
    Q_INVOKABLE bool saveAs();
    Q_INVOKABLE void quit();
    Q_INVOKABLE bool canClose();

    Q_INVOKABLE void markModified();
    Q_INVOKABLE QString pickImageFile();
    Q_INVOKABLE QString pickPdfExportFile();
    Q_INVOKABLE QString pickSaveFolder();
    Q_INVOKABLE QString documentDirectory() const;
    Q_INVOKABLE QString markdownPath(const QString &absolutePath) const;
    Q_INVOKABLE QString suggestedSavePath() const;

    Q_INVOKABLE bool clipboardHasImage() const;
    Q_INVOKABLE QString pasteImageFromClipboard();
    Q_INVOKABLE bool defaultPackageOnSave() const;
    Q_INVOKABLE bool performSave(const QString &path, bool packageAssets);
    Q_INVOKABLE void discardChanges();

Q_SIGNALS:
    void textChanged();
    void filePathChanged();
    void modifiedChanged();
    void windowTitleChanged();
    void recentFilesChanged();
    void prefersPackagingChanged();
    void unstagedAssetCountChanged();
    void saveDialogRequested(bool saveAs,
                             const QString &suggestedPath,
                             bool defaultPackage,
                             int unstagedCount,
                             bool showDiscard);

private:
    bool confirmSave();
    bool writeToFile(const QString &path);
    bool readFromFile(const QString &path);
    void setModified(bool modified);
    void setFilePath(const QString &path);
    void setContent(const QString &text, bool clean);
    void loadRecentFiles();
    void saveRecentFiles();
    void addRecentFile(const QString &path);
    void syncAssetsState();
    void scheduleAssetSync();
    void updatePackagingPreference();
    bool quickSave();
    bool requestSaveDialog(bool saveAs, bool showDiscard = false);
    QString normalizeMarkdownPath(const QString &path) const;
    bool ensureMarkdownExtension(QString &path) const;

    QString m_text;
    QString m_filePath;
    bool m_modified = false;
    bool m_blockModified = false;
    bool m_prefersPackaging = true;
    int m_unstagedAssetCount = 0;
    QStringList m_recentFiles;
    DocumentAssets m_assets;
    QTimer m_assetSyncTimer;
};
