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
    QApplication::setDesktopFileName(QStringLiteral("org.kde.simplemd"));

    QIcon appIcon = QIcon::fromTheme(QStringLiteral("org.kde.simplemd"));
    if (appIcon.isNull()) {
        appIcon = QIcon(QStringLiteral(":/icons/org.kde.simplemd.png"));
    }
    QApplication::setWindowIcon(appIcon);

    QApplication::setStyle(QStringLiteral("breeze"));
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }

    qmlRegisterType<DocumentController>("org.kde.simplemd", 1, 0, "DocumentController");
    qmlRegisterType<LlmClient>("org.kde.simplemd", 1, 0, "LlmClient");
    qmlRegisterType<PreviewHelper>("org.kde.simplemd", 1, 0, "PreviewHelper");
    qmlRegisterType<PrintHelper>("org.kde.simplemd", 1, 0, "PrintHelper");
    qmlRegisterType<EditorHelper>("org.kde.simplemd", 1, 0, "EditorHelper");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.loadFromModule(QStringLiteral("org.kde.simplemd"), QStringLiteral("Main"));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
