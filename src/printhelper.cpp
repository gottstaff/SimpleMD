#include "printhelper.h"

#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QPainter>
#include <QPrintDialog>
#include <QPdfDocument>
#include <QPrinter>
#include <QStandardPaths>
#include <QUuid>

namespace {
void printPdfDocument(const QString &pdfPath, QPrinter &printer)
{
    QPdfDocument document;
    if (document.load(pdfPath) != QPdfDocument::Error::None
        || document.status() != QPdfDocument::Status::Ready) {
        qWarning("PrintHelper: could not load PDF");
        return;
    }

    QPainter painter;
    if (!painter.begin(&printer)) {
        qWarning("PrintHelper: could not start printing");
        return;
    }

    const int pageCount = document.pageCount();
    for (int page = 0; page < pageCount; ++page) {
        if (page > 0) {
            printer.newPage();
        }

        const QSizeF pageSize = document.pagePointSize(page);
        if (pageSize.isEmpty()) {
            continue;
        }

        const QRect target = painter.viewport();
        const QSize renderSize(
            qMax(1, int(double(target.width()) * pageSize.width() / pageSize.height())),
            target.height());
        const QImage image = document.render(page, renderSize);
        painter.drawImage(target, image);
    }

    painter.end();
}
} // namespace

PrintHelper::PrintHelper(QObject *parent)
    : QObject(parent)
{
}

void PrintHelper::onPdfPrintingFinished(const QString &filePath, bool ok)
{
    if (filePath != m_pdfPath) {
        return;
    }

    m_success = ok;
    if (ok && m_printer) {
        printPdfDocument(filePath, *m_printer);
    }
    QFile::remove(filePath);

    if (m_loop) {
        m_loop->quit();
    }
}

void PrintHelper::printPreview(QObject *webEngineView)
{
    if (!webEngineView) {
        qWarning("PrintHelper: expected a WebEngineView");
        return;
    }

    if (webEngineView->metaObject()->indexOfMethod("printToPdf(QString)") < 0) {
        qWarning("PrintHelper: object does not support printToPdf");
        return;
    }

    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dialog(&printer);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    m_pdfPath = QDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation))
                    .filePath(QStringLiteral("simplemd-print-%1.pdf")
                                  .arg(QUuid::createUuid().toString(QUuid::WithoutBraces)));
    m_printer = &printer;
    m_success = false;

    QEventLoop loop;
    m_loop = &loop;

    const QMetaObject::Connection connection = QObject::connect(
        webEngineView,
        SIGNAL(pdfPrintingFinished(QString, bool)),
        this,
        SLOT(onPdfPrintingFinished(QString, bool)));

    QMetaObject::invokeMethod(webEngineView, "printToPdf", Q_ARG(QString, m_pdfPath));
    loop.exec();
    QObject::disconnect(connection);

    m_loop.clear();
    m_printer = nullptr;

    if (!m_success) {
        QFile::remove(m_pdfPath);
        qWarning("PrintHelper: PDF generation failed");
    }
    m_pdfPath.clear();
}
