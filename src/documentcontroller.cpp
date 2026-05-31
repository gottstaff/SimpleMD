#include "documentcontroller.h"

#include "imageresolver.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>
#include <QStringConverter>
#include <QTextStream>

#include <KLocalizedString>

namespace
{
QString fileDialogFilter()
{
    return i18n("Markdown files (*.md *.markdown *.mdown);;Text files (*.txt);;All files (*)");
}

constexpr int kMaxRecentFiles = 10;
constexpr QLatin1StringView kRecentFilesKey("paths");
constexpr QLatin1StringView kRecentFilesGroup("RecentFiles");
}

DocumentController::DocumentController(QObject *parent)
    : QObject(parent)
{
    m_assetSyncTimer.setSingleShot(true);
    m_assetSyncTimer.setInterval(400);
    connect(&m_assetSyncTimer, &QTimer::timeout, this, &DocumentController::syncAssetsState);

    loadRecentFiles();
    syncAssetsState();
}

QString DocumentController::text() const
{
    return m_text;
}

void DocumentController::setText(const QString &text)
{
    if (m_text == text) {
        return;
    }
    m_text = text;
    if (!m_blockModified) {
        setModified(true);
    }
    scheduleAssetSync();
    Q_EMIT textChanged();
}

void DocumentController::scheduleAssetSync()
{
    m_assetSyncTimer.start();
}

QString DocumentController::filePath() const
{
    return m_filePath;
}

bool DocumentController::modified() const
{
    return m_modified;
}

QString DocumentController::windowTitle() const
{
    const QString name = m_filePath.isEmpty() ? i18n("Untitled") : QFileInfo(m_filePath).fileName();
    return m_modified ? i18n("%1 [*]", name) : name;
}

QStringList DocumentController::recentFiles() const
{
    return m_recentFiles;
}

bool DocumentController::prefersPackaging() const
{
    return m_prefersPackaging;
}

int DocumentController::unstagedAssetCount() const
{
    return m_unstagedAssetCount;
}

void DocumentController::markModified()
{
    setModified(true);
}

QString DocumentController::pickImageFile()
{
    const QString path = QFileDialog::getOpenFileName(
        nullptr,
        i18n("Insert Image"),
        documentDirectory().isEmpty() ? QDir::homePath() : documentDirectory(),
        i18n("Images (*.png *.jpg *.jpeg *.svg *.webp *.gif);;All files (*)"));
    return path;
}

QString DocumentController::pickPdfExportFile()
{
    QString suggested = m_filePath.isEmpty()
        ? QDir::homePath() + QStringLiteral("/untitled.pdf")
        : QFileInfo(m_filePath).completeBaseName() + QStringLiteral(".pdf");

    QString path = QFileDialog::getSaveFileName(
        nullptr,
        i18n("Export to PDF"),
        suggested,
        i18n("PDF files (*.pdf);;All files (*)"));
    if (path.isEmpty()) {
        return {};
    }

    if (!path.endsWith(QLatin1String(".pdf"), Qt::CaseInsensitive)) {
        path += QStringLiteral(".pdf");
    }

    return path;
}

QString DocumentController::pickSaveFolder()
{
    const QString startDir = m_filePath.isEmpty()
        ? QDir::homePath()
        : QFileInfo(m_filePath).absolutePath();
    return QFileDialog::getExistingDirectory(nullptr, i18n("Choose Folder"), startDir);
}

QString DocumentController::documentDirectory() const
{
    if (m_filePath.isEmpty()) {
        return {};
    }
    return QFileInfo(m_filePath).absolutePath();
}

QString DocumentController::markdownPath(const QString &absolutePath) const
{
    if (absolutePath.isEmpty()) {
        return {};
    }

    const QFileInfo imageInfo(absolutePath);
    if (!imageInfo.exists()) {
        return absolutePath;
    }

    const QString docDir = documentDirectory();
    if (docDir.isEmpty()) {
        return QString(imageInfo.absoluteFilePath()).replace(QLatin1Char('\\'), QLatin1Char('/'));
    }

    const QString relative = QDir(docDir).relativeFilePath(imageInfo.absoluteFilePath());
    return QString(relative).replace(QLatin1Char('\\'), QLatin1Char('/'));
}

QString DocumentController::suggestedSavePath() const
{
    if (!m_filePath.isEmpty()) {
        return m_filePath;
    }

    return QDir::homePath() + QStringLiteral("/untitled/untitled.md");
}

bool DocumentController::clipboardHasImage() const
{
    return m_assets.clipboardHasImage();
}

QString DocumentController::pasteImageFromClipboard()
{
    return m_assets.saveClipboardImage();
}

bool DocumentController::defaultPackageOnSave() const
{
    if (m_filePath.isEmpty()) {
        return true;
    }

    return m_prefersPackaging;
}

void DocumentController::syncAssetsState()
{
    m_assets.setDocumentDirectory(documentDirectory());

    const int count = m_assets.listUnstagedAssets(m_text, documentDirectory()).size();
    if (m_unstagedAssetCount != count) {
        m_unstagedAssetCount = count;
        Q_EMIT unstagedAssetCountChanged();
    }
}

void DocumentController::updatePackagingPreference()
{
    const bool packaged = m_filePath.isEmpty()
        ? true
        : m_assets.isDocumentPackaged(m_text, documentDirectory());

    if (m_prefersPackaging != packaged) {
        m_prefersPackaging = packaged;
        Q_EMIT prefersPackagingChanged();
    }
}

bool DocumentController::newDocument()
{
    if (!confirmSave()) {
        return false;
    }
    setFilePath({});
    setContent({}, true);
    m_assets.setDocumentDirectory({});
    updatePackagingPreference();
    syncAssetsState();
    Q_EMIT filePathChanged();
    Q_EMIT windowTitleChanged();
    return true;
}

bool DocumentController::open()
{
    if (!confirmSave()) {
        return false;
    }

    const QString path = QFileDialog::getOpenFileName(
        nullptr,
        i18n("Open File"),
        m_filePath.isEmpty() ? QDir::homePath() : QFileInfo(m_filePath).absolutePath(),
        fileDialogFilter());
    if (path.isEmpty()) {
        return false;
    }

    return openFile(path);
}

bool DocumentController::openFile(const QString &path)
{
    if (path.isEmpty()) {
        return false;
    }

    const QString canonical = QFileInfo(path).absoluteFilePath();
    if (!QFileInfo::exists(canonical)) {
        QMessageBox::warning(nullptr,
                             i18n("Open Error"),
                             i18n("File not found:\n%1", canonical));
        return false;
    }

    if (!readFromFile(canonical)) {
        return false;
    }

    setFilePath(canonical);
    addRecentFile(canonical);
    m_assets.setDocumentDirectory(documentDirectory());
    updatePackagingPreference();
    syncAssetsState();
    Q_EMIT filePathChanged();
    Q_EMIT windowTitleChanged();
    return true;
}

bool DocumentController::openRecent(const QString &path)
{
    if (path.isEmpty()) {
        return false;
    }

    if (!confirmSave()) {
        return false;
    }

    const QString canonical = QFileInfo(path).absoluteFilePath();
    if (!QFileInfo::exists(canonical)) {
        m_recentFiles.removeAll(canonical);
        saveRecentFiles();
        Q_EMIT recentFilesChanged();
        QMessageBox::warning(nullptr,
                             i18n("Open Error"),
                             i18n("File not found:\n%1", canonical));
        return false;
    }

    return openFile(canonical);
}

bool DocumentController::ensureMarkdownExtension(QString &path) const
{
    if (path.isEmpty()) {
        return false;
    }

    if (!path.endsWith(QLatin1String(".md"), Qt::CaseInsensitive)
        && !path.endsWith(QLatin1String(".markdown"), Qt::CaseInsensitive)
        && !path.endsWith(QLatin1String(".mdown"), Qt::CaseInsensitive)
        && !path.endsWith(QLatin1String(".txt"), Qt::CaseInsensitive)) {
        path += QStringLiteral(".md");
    }

    return true;
}

bool DocumentController::requestSaveDialog(bool saveAs, bool showDiscard)
{
    Q_EMIT saveDialogRequested(saveAs,
                               suggestedSavePath(),
                               defaultPackageOnSave(),
                               m_unstagedAssetCount,
                               showDiscard);
    return false;
}

bool DocumentController::quickSave()
{
    if (m_filePath.isEmpty()) {
        return false;
    }

    const bool shouldPackage = m_prefersPackaging
        && m_assets.hasExternalAssets(m_text, documentDirectory());

    if (shouldPackage) {
        const PackageSaveResult result = m_assets.packageAndSave(m_text, m_filePath, true);
        if (!result.success) {
            QMessageBox::warning(nullptr,
                                 i18n("Save Error"),
                                 i18n("Could not save file: %1", result.errorMessage));
            return false;
        }

        if (result.markdown != m_text) {
            setContent(result.markdown, false);
        }
    } else if (!writeToFile(m_filePath)) {
        return false;
    }

    setModified(false);
    Q_EMIT windowTitleChanged();
    addRecentFile(m_filePath);
    updatePackagingPreference();
    syncAssetsState();
    return true;
}

bool DocumentController::save()
{
    if (m_filePath.isEmpty() || m_unstagedAssetCount > 0) {
        return requestSaveDialog(false, false);
    }

    return quickSave();
}

bool DocumentController::saveAs()
{
    return requestSaveDialog(true, false);
}

void DocumentController::discardChanges()
{
    setModified(false);
    Q_EMIT windowTitleChanged();
}

bool DocumentController::performSave(const QString &path, bool packageAssets)
{
    QString normalizedPath = normalizeMarkdownPath(path);
    if (normalizedPath.isEmpty()) {
        return false;
    }

    ensureMarkdownExtension(normalizedPath);

    QDir().mkpath(QFileInfo(normalizedPath).absolutePath());

    const PackageSaveResult result = m_assets.packageAndSave(m_text, normalizedPath, packageAssets);
    if (!result.success) {
        QMessageBox::warning(nullptr,
                             i18n("Save Error"),
                             i18n("Could not save file: %1", result.errorMessage));
        return false;
    }

    if (result.markdown != m_text) {
        setContent(result.markdown, false);
    }

    setFilePath(normalizedPath);
    m_assets.setDocumentDirectory(documentDirectory());
    setModified(false);
    m_prefersPackaging = packageAssets;
    Q_EMIT prefersPackagingChanged();
    addRecentFile(normalizedPath);
    syncAssetsState();
    Q_EMIT filePathChanged();
    Q_EMIT windowTitleChanged();
    return true;
}

QString DocumentController::normalizeMarkdownPath(const QString &path) const
{
    if (path.isEmpty()) {
        return {};
    }

    return QFileInfo(path).absoluteFilePath();
}

void DocumentController::quit()
{
    if (!confirmSave()) {
        return;
    }
    QApplication::quit();
}

bool DocumentController::canClose()
{
    return confirmSave();
}

bool DocumentController::confirmSave()
{
    if (!m_modified) {
        return true;
    }

    if (m_unstagedAssetCount > 0) {
        return requestSaveDialog(false, true);
    }

    QMessageBox box(QMessageBox::Warning,
                    i18n("Unsaved Changes"),
                    i18n("The document has been modified. Do you want to save your changes?"));
    box.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    box.setDefaultButton(QMessageBox::Save);

    switch (box.exec()) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Discard:
        return true;
    default:
        return false;
    }
}

bool DocumentController::writeToFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, i18n("Save Error"), i18n("Could not save file: %1", file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << m_text;
    return true;
}

bool DocumentController::readFromFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, i18n("Open Error"), i18n("Could not open file: %1", file.errorString()));
        return false;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    setContent(in.readAll(), true);
    return true;
}

void DocumentController::setModified(bool modified)
{
    if (m_modified == modified) {
        return;
    }
    m_modified = modified;
    Q_EMIT modifiedChanged();
    Q_EMIT windowTitleChanged();
}

void DocumentController::setFilePath(const QString &path)
{
    if (m_filePath == path) {
        return;
    }
    m_filePath = path;
    Q_EMIT filePathChanged();
}

void DocumentController::setContent(const QString &text, bool clean)
{
    m_blockModified = true;
    m_text = text;
    m_blockModified = false;
    Q_EMIT textChanged();
    setModified(!clean);
    syncAssetsState();
    if (!clean) {
        updatePackagingPreference();
    }
}

void DocumentController::loadRecentFiles()
{
    QSettings settings;
    settings.beginGroup(kRecentFilesGroup);
    const QStringList stored = settings.value(kRecentFilesKey).toStringList();
    settings.endGroup();

    QStringList valid;
    valid.reserve(stored.size());
    for (const QString &path : stored) {
        const QString canonical = QFileInfo(path).absoluteFilePath();
        if (!canonical.isEmpty() && QFileInfo::exists(canonical) && !valid.contains(canonical)) {
            valid.append(canonical);
        }
    }

    m_recentFiles = valid;
    if (valid != stored) {
        saveRecentFiles();
    }
}

void DocumentController::saveRecentFiles()
{
    QSettings settings;
    settings.beginGroup(kRecentFilesGroup);
    settings.setValue(kRecentFilesKey, m_recentFiles);
    settings.endGroup();
}

void DocumentController::addRecentFile(const QString &path)
{
    if (path.isEmpty()) {
        return;
    }

    const QString canonical = QFileInfo(path).absoluteFilePath();
    if (canonical.isEmpty() || !QFileInfo::exists(canonical)) {
        return;
    }

    m_recentFiles.removeAll(canonical);
    m_recentFiles.prepend(canonical);
    while (m_recentFiles.size() > kMaxRecentFiles) {
        m_recentFiles.removeLast();
    }

    saveRecentFiles();
    Q_EMIT recentFilesChanged();
}
