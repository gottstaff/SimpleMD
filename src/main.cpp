#include <QApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>
#include <QQuickStyle>
#include <QtWebEngineQuick/qtwebenginequickglobal.h>
#include <cstdlib>

#include "documentcontroller.h"
#include "editorhelper.h"
#include "previewhelper.h"
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
    qmlRegisterType<PrintHelper>("io.github.gottstaff.SimpleMD", 1, 0, "PrintHelper");
    qmlRegisterType<EditorHelper>("io.github.gottstaff.SimpleMD", 1, 0, "EditorHelper");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.loadFromModule(QStringLiteral("io.github.gottstaff.SimpleMD"), QStringLiteral("Main"));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
