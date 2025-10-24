#ifndef ICONIMAGEPROVIDER_H
#define ICONIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QIcon>
#include <QUrl>
#include <QColor>

class IconImageProvider : public QQuickImageProvider
{
public:
    IconImageProvider();

    static QString getNameFromQueryItem(const QString &query);
    static QColor getColorFromQueryItem(const QString &query);

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;
};

#endif // ICONIMAGEPROVIDER_H
