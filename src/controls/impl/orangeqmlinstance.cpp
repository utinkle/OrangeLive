#include "orangeqmlinstance.h"

#include <QQuickWindow>
#include <QUrlQuery>

OrangeQmlInstance::OrangeQmlInstance(QObject *parent)
    : QObject{parent}
{}

QUrl OrangeQmlInstance::toTokIconUrl(const QString &name, const QColor &color)
{
    QUrl url;
    url.setScheme("image");
    url.setHost("toklive.icons");

    QUrlQuery args;
    args.addQueryItem("name", name);
    args.addQueryItem("color", color.name());

    url.setQuery(args);

    return url;
}

