#include "iconimageprovider.h"
#include <QIcon>
#include <QPainter>
#include <QPixmap>
#include <QPixmapCache>
#include <QUrlQuery>

IconImageProvider::IconImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{ }

QString IconImageProvider::getNameFromQueryItem(const QString &query)
{
    QUrlQuery args(query);

    return args.queryItemValue("name");
}

QColor IconImageProvider::getColorFromQueryItem(const QString &query)
{
    QUrlQuery args(query);

    return QColor(args.queryItemValue("color"));
}

QPixmap IconImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    const QString &iconName = getNameFromQueryItem(id);
    const QColor &iconColor = getColorFromQueryItem(id);
    bool colorIsValid = iconColor.isValid() && iconColor.alpha() > 0;

    QSize iconSize = requestedSize.isValid() ? requestedSize : QSize(32, 32);
    const QString &sizeString = QStringLiteral("%1x%2").arg(iconSize.width()).arg(iconSize.height());
    QString key = QStringLiteral("_tok_icon_cached:%1:%2").arg(iconName, sizeString);
    if (colorIsValid) {
        key += QLatin1String(":") + iconColor.name();
    }

    QPixmap pixmap;
    if (!QPixmapCache::find(key, &pixmap)) {
        QIcon icon = QIcon::fromTheme(iconName);
        if (icon.isNull()) {
            return pixmap;
        }

        pixmap = icon.pixmap(iconSize);
        if (pixmap.isNull())
            return pixmap;

        if (colorIsValid) {
            QPainter painter(&pixmap);
            painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            painter.fillRect(pixmap.rect(), iconColor);
        }

        if (pixmap.size() !=  iconSize) {
            pixmap = pixmap.scaled(iconSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }

        QPixmapCache::insert(key, pixmap);
    }

    if (size) {
        *size = pixmap.size();
    }

    return pixmap;
}
