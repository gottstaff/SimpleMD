#pragma once

#include <QObject>
#include <QString>

class PreviewImageBridge : public QObject
{
    Q_OBJECT

public:
    explicit PreviewImageBridge(QObject *parent = nullptr);

    Q_INVOKABLE QString loadImageAsDataUrl(const QString &absolutePath) const;
};
