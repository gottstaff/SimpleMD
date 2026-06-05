#include <QApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>
#include <QQuickStyle>
#include <QtWebEngineQuick/qtwebenginequickglobal.h>
#include <cstdlib>

#include "appintegration.h"
#include "credentialstore.h"
#include "documentcontroller.h"
#include "editorhelper.h"
#include "previewhelper.h"
#include "previewimagebridge.h"
#include "pdfexportthemes.h"
#include "printhelper.h"
#include "llmclient.h"
#include <QUrl>

#include <KIconTheme>
#include <KLocalizedContext>
#include <KLocalizedString>

int main(int argc, char *argv[])
{
    if (qEnvironmentVariableIsEmpty("QTWEBENGINE_CHROMIUM_FLAGS")) {
        qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--disable-gpu-compositing");
    }

    KIconTheme::initTheme();
    QtWebEngineQuick::initialize();

    QApplication app(argc, argv);

    const QStringList arguments = app.arguments();
    if (AppIntegration::wantsHelp(arguments)) {
        AppIntegration::printHelp();
        return 0;
    }

    const QStringList initialFiles = AppIntegration::collectFilePaths(arguments.mid(1));
    AppIntegration appIntegration;
    const QString instanceKey = QStringLiteral("io.github.gottstaff.SimpleMD");
    if (!appIntegration.ensurePrimaryInstance(instanceKey)) {
        appIntegration.sendToRunningInstance(instanceKey, initialFiles);
        return 0;
    }
    appIntegration.setPendingFiles(initialFiles);

    KLocalizedString::setApplicationDomain("simplemd");
    QApplication::setOrganizationName(QStringLiteral("KDE"));
    QApplication::setOrganizationDomain(QStringLiteral("kde.org"));
    QApplication::setApplicationName(QStringLiteral("SimpleMD"));
    QApplication::setDesktopFileName(QStringLiteral("io.github.gottstaff.SimpleMD"));

    QIcon appIcon = QIcon::fromTheme(QStringLiteral("io.github.gottstaff.SimpleMD"));
    if (appIcon.isNull()) {
        appIcon = QIcon(QStringLiteral(":/icons/io.github.gottstaff.SimpleMD.png"));
    }
    QApplication::setWindowIcon(appIcon);

    QApplication::setStyle(QStringLiteral("breeze"));
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }

    qmlRegisterType<DocumentController>("io.github.gottstaff.SimpleMD", 1, 0, "DocumentController");
    qmlRegisterType<LlmClient>("io.github.gottstaff.SimpleMD", 1, 0, "LlmClient");
    qmlRegisterType<PreviewHelper>("io.github.gottstaff.SimpleMD", 1, 0, "PreviewHelper");
    qmlRegisterType<PreviewImageBridge>("io.github.gottstaff.SimpleMD", 1, 0, "PreviewImageBridge");
    qmlRegisterType<PrintHelper>("io.github.gottstaff.SimpleMD", 1, 0, "PrintHelper");
    qmlRegisterType<PdfExportThemes>("io.github.gottstaff.SimpleMD", 1, 0, "PdfExportThemes");
    qmlRegisterType<CredentialStore>("io.github.gottstaff.SimpleMD", 1, 0, "CredentialStore");
    qmlRegisterType<EditorHelper>("io.github.gottstaff.SimpleMD", 1, 0, "EditorHelper");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.rootContext()->setContextProperty(QStringLiteral("appIntegration"), &appIntegration);
    engine.loadFromModule(QStringLiteral("io.github.gottstaff.SimpleMD"), QStringLiteral("Main"));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
