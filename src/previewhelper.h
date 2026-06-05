#pragma once

#include <QObject>

class PreviewHelper : public QObject
{
    Q_OBJECT

public:
    explicit PreviewHelper(QObject *parent = nullptr);

    Q_INVOKABLE QString buildPreviewScript(const QString &markdown,
                                           const QString &textColor,
                                           const QString &backgroundColor,
                                           const QString &linkColor,
                                           const QString &fontFamily,
                                           int fontSize,
                                           double lineHeight,
                                           int padPx,
                                           int maxWidthEm,
                                           const QString &documentDirectory = QString(),
                                           const QString &stagingDirectory = QString(),
                                           const QString &scrollbarThumb = QString(),
                                           const QString &scrollbarTrack = QString(),
                                           const QString &scrollbarThumbHover = QString(),
                                           const QString &scrollbarThumbActive = QString()) const;
};
