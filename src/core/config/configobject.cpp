#include "configobject.h"
#include "config.h"

#include <QMetaProperty>
#include <QMetaMethod>
#include <QDynamicPropertyChangeEvent>
#include <QDebug>

class PropertyNotifyConnector : public QObject
{
    Q_OBJECT

public:
    PropertyNotifyConnector(QObject *parent, const QByteArray &propertyName)
        : QObject(parent)
        , m_propertyName(propertyName)
    {
    }

public slots:
    void onPropertyChanged()
    {
        QObject *parentObj = parent();
        if (!parentObj) {
            return;
        }

        QMetaObject::invokeMethod(parentObj,
                                  "handleStaticPropertyNotify",
                                  Q_ARG(QByteArray, m_propertyName));
    }

private:
    QByteArray m_propertyName;
};

ConfigObject::ConfigObject(QObject *parent)
    : QObject(parent)
{
    initialize();
}

void ConfigObject::initialize()
{
}

void ConfigObject::connectSignalsAndAttachParent()
{
    const QMetaObject *metaObj = metaObject();
    const int propertyOffset = metaObj->propertyOffset();

    for (int i = propertyOffset; i < metaObj->propertyCount(); ++i) {
        const QMetaProperty property = metaObj->property(i);
        attachIfConfigObject(property.name(), this->property(property.name()));

        if (!property.hasNotifySignal()) {
            continue;
        }

        const QMetaMethod notifySignal = property.notifySignal();
        PropertyNotifyConnector *connector = new PropertyNotifyConnector(this, property.name());
        const QMetaMethod notifySlot = connector->metaObject()->method(connector->metaObject()->indexOfSlot("onPropertyChanged()"));

        const bool connected = QObject::connect(this, notifySignal, connector, notifySlot);

        if (!connected) {
            qWarning() << "Failed to connect notify signal for property:" << property.name();
            connector->deleteLater();
        }
    }
}

void ConfigObject::handleStaticPropertyNotify(const QByteArray &propName)
{
    if (m_loading.load(std::memory_order_acquire)) {
        return;
    }

    const QVariant value = property(propName);
    handlePropertyChange(propName, value);
}

void ConfigObject::handlePropertyChange(const QString &name, const QVariant &value)
{
    if (!m_loading.load(std::memory_order_acquire)) {
        QWriteLocker locker(&m_lock);
        attachIfConfigObject(name, value);
    }

    Q_EMIT valueChanged(name, value);
    Q_EMIT changed();

    if (m_rootConfig) {
        Q_EMIT m_rootConfig->valueChanged(name, value);
    }
}

bool ConfigObject::event(QEvent *event)
{
    if (event->type() == QEvent::DynamicPropertyChange) {
        auto dynamicEvent = static_cast<QDynamicPropertyChangeEvent *>(event);
        handleDynamicPropertyChange(dynamicEvent->propertyName());
        return true;
    }

    return QObject::event(event);
}

void ConfigObject::classBegin()
{

}

void ConfigObject::componentComplete()
{
    connectSignalsAndAttachParent();
}

void ConfigObject::handleDynamicPropertyChange(const QByteArray &propertyName)
{
    if (m_loading.load(std::memory_order_acquire)) {
        return;
    }

    const QString name = QString::fromUtf8(propertyName);
    const QVariant value = property(propertyName.constData());
    handlePropertyChange(name, value);
}

void ConfigObject::attachIfConfigObject(const QString &key, const QVariant &value)
{
    if (value.userType() != qMetaTypeId<ConfigObject *>()) {
        return;
    }

    QObject *obj = value.value<QObject *>();
    if (!obj) {
        return;
    }

    ConfigObject *child = qobject_cast<ConfigObject *>(obj);
    if (!child) {
        return;
    }

    if (m_rootConfig) {
        child->setRootConfig(m_rootConfig);
    }

    connect(child, &ConfigObject::changed,
            this, &ConfigObject::changed, Qt::UniqueConnection);
    connect(child, &ConfigObject::valueChanged,
            this, &ConfigObject::valueChanged, Qt::UniqueConnection);
}

void ConfigObject::setRootConfig(Config *cfg)
{
    m_rootConfig = cfg;
    propagateRootConfigToChildren(cfg);
}

void ConfigObject::propagateRootConfigToChildren(Config *cfg)
{
    const QMetaObject *metaObj = metaObject();
    const int propertyOffset = metaObj->propertyOffset();

    for (int i = propertyOffset; i < metaObj->propertyCount(); ++i) {
        const QMetaProperty property = metaObj->property(i);
        if (!property.isReadable()) {
            continue;
        }

        const QVariant value = property.read(this);
        if (value.userType() != qMetaTypeId<ConfigObject *>()) {
            continue;
        }

        QObject *obj = value.value<QObject *>();
        ConfigObject *child = qobject_cast<ConfigObject *>(obj);
        if (child) {
            child->setRootConfig(cfg);
        }
    }

    const auto dynamicProps = dynamicPropertyNames();
    for (const QByteArray &dynamicProp : dynamicProps) {
        const QVariant value = property(dynamicProp.constData());
        if (value.userType() != qMetaTypeId<ConfigObject *>()) {
            continue;
        }

        QObject *obj = value.value<QObject *>();
        ConfigObject *child = qobject_cast<ConfigObject *>(obj);
        if (child) {
            child->setRootConfig(cfg);
        }
    }
}

Config *ConfigObject::rootConfig() const
{
    return m_rootConfig;
}

QVariantMap ConfigObject::toVariantMap() const
{
    if (m_loading.load(std::memory_order_acquire)) {
        return QVariantMap();
    }

    QReadLocker locker(&m_lock);
    QVariantMap result;

    serializeStaticProperties(result);
    serializeDynamicProperties(result);

    return result;
}

void ConfigObject::serializeStaticProperties(QVariantMap &out) const
{
    const QMetaObject *metaObj = metaObject();
    const int propertyOffset = metaObj->propertyOffset();

    for (int i = propertyOffset; i < metaObj->propertyCount(); ++i) {
        const QMetaProperty property = metaObj->property(i);
        if (!property.isReadable()) {
            continue;
        }

        const QString name = QString::fromUtf8(property.name());
        const QVariant value = property.read(this);

        if (value.userType() == qMetaTypeId<ConfigObject *>()) {
            QObject *obj = value.value<QObject *>();
            ConfigObject *child = qobject_cast<ConfigObject *>(obj);
            if (child) {
                out.insert(name, child->toVariantMap());
                continue;
            }
        }

        out.insert(name, value);
    }
}

void ConfigObject::serializeDynamicProperties(QVariantMap &out) const
{
    const auto dynamicProps = dynamicPropertyNames();
    for (const QByteArray &dynamicProp : dynamicProps) {
        const QString name = QString::fromUtf8(dynamicProp);
        const QVariant value = property(dynamicProp.constData());

        if (value.userType() == qMetaTypeId<ConfigObject *>()) {
            QObject *obj = value.value<QObject *>();
            ConfigObject *child = qobject_cast<ConfigObject *>(obj);
            if (child) {
                out.insert(name, child->toVariantMap());
                continue;
            }
        }

        out.insert(name, value);
    }
}

void ConfigObject::fromVariantMap(const QVariantMap &map)
{
    m_loading.store(true, std::memory_order_release);

    QWriteLocker locker(&m_lock);

    for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
        deserializeProperty(it.key(), it.value());
    }

    locker.unlock();
    m_loading.store(false, std::memory_order_release);
}

void ConfigObject::deserializeProperty(const QString &key, const QVariant &value)
{
    const QByteArray keyBytes = key.toUtf8();
    const QMetaObject *metaObj = metaObject();
    const int propertyIndex = metaObj->indexOfProperty(keyBytes);

    if (propertyIndex >= 0) {
        const QMetaProperty property = metaObj->property(propertyIndex);

        if (value.typeId() == QMetaType::QVariantMap
            && property.userType() == qMetaTypeId<ConfigObject *>()) {
            const QVariant existing = property.read(this);
            if (existing.userType() == qMetaTypeId<ConfigObject *>()) {
                QObject *obj = existing.value<QObject *>();
                ConfigObject *child = qobject_cast<ConfigObject *>(obj);
                if (child) {
                    child->fromVariantMap(value.toMap());
                    return;
                }
            }

            ConfigObject *child = new ConfigObject(this);
            child->fromVariantMap(value.toMap());
            child->setRootConfig(m_rootConfig);
            setProperty(keyBytes.constData(), QVariant::fromValue(child));
        } else {
            setProperty(keyBytes.constData(), value);
        }
    } else {
        if (value.typeId() == QMetaType::QVariantMap) {
            ConfigObject *child = new ConfigObject(this);
            child->fromVariantMap(value.toMap());
            child->setRootConfig(m_rootConfig);
            setProperty(keyBytes.constData(), QVariant::fromValue(child));
        } else {
            setProperty(keyBytes.constData(), value);
        }
    }
}

#include "configobject.moc"
