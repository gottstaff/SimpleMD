#pragma once

#include <QObject>
#include <QPointer>
#include <QPrinter>
#include <QString>

class QEventLoop;

class PrintHelper : public QObject
{
    Q_OBJECT

public:
    explicit PrintHelper(QObject *parent = nullptr);

    Q_INVOKABLE void printPreview(QObject *webEngineView);

private Q_SLOTS:
    void onPdfPrintingFinished(const QString &filePath, bool ok);

private:
    QString m_pdfPath;
    QPrinter *m_printer = nullptr;
    bool m_success = false;
    QPointer<QEventLoop> m_loop;
};
