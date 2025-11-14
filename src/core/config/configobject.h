#ifndef CONFIGOBJECT_H
#define CONFIGOBJECT_H

#include <QObject>
#include <QVariantMap>
#include <QReadWriteLock>
#include <QQmlParserStatus>

class Config;

class ConfigObject : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

public:
    explicit ConfigObject(QObject *parent = nullptr);

    virtual QVariantMap toVariantMap() const;
    virtual void fromVariantMap(const QVariantMap &map);

    void setRootConfig(Config *cfg);
    Config *rootConfig() const;

Q_SIGNALS:
    void changed();
    void valueChanged(const QString &propertyName, const QVariant &newValue);

protected:
    bool event(QEvent *event) override;
    void classBegin() override;
    void componentComplete() override;

private Q_SLOTS:
    void handleStaticPropertyNotify(const QByteArray &propName);

private:
    void initialize();
    void connectSignalsAndAttachParent();
    void handlePropertyChange(const QString &name, const QVariant &value);
    void handleDynamicPropertyChange(const QByteArray &propertyName);
    void attachIfConfigObject(const QString &key, const QVariant &value);
    void propagateRootConfigToChildren(Config *cfg);
    void serializeStaticProperties(QVariantMap &out) const;
    void serializeDynamicProperties(QVariantMap &out) const;
    void deserializeProperty(const QString &key, const QVariant &value);

    Config *m_rootConfig = nullptr;
    mutable QReadWriteLock m_lock;
    std::atomic<bool> m_loading{false};
};

#endif // CONFIGOBJECT_H
