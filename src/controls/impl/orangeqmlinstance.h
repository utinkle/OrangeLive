#ifndef ORANGEQMLINSTANCE_H
#define ORANGEQMLINSTANCE_H

#include <QObject>
#include <QUrl>
#include <QQmlComponent>
#include <QColor>

class OrangeQmlInstance : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT("OrangeLive")
    QML_SINGLETON

public:
    explicit OrangeQmlInstance(QObject *parent = nullptr);

    Q_INVOKABLE QUrl toTokIconUrl(const QString &name, const QColor &color);
};

#endif // ORANGEQMLINSTANCE_H
