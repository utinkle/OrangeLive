#include "config_manager.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QStandardPaths>

#include <QDebug>

ConfigManager::ConfigManager(QObject *parent)
    : QQmlPropertyMap(this, parent)
    , m_name("app")
    , m_group("default")
    , m_loading(false)
    , m_initialized(false)
{
}

ConfigManager::~ConfigManager()
{
    if (m_initialized) {
        saveConfig();
    }
}

QString ConfigManager::name() const
{
    return m_name;
}

void ConfigManager::setName(const QString &name)
{
    if (m_name != name && !name.isEmpty()) {
        if (m_initialized) {
            saveConfig();
        }

        m_name = name;

        if (!m_group.isEmpty()) {
            loadConfig();
            m_initialized = true;
        }

        Q_EMIT nameChanged();
    }
}

QString ConfigManager::group() const
{
    return m_group;
}

void ConfigManager::setGroup(const QString &group)
{
    if (m_group != group && !group.isEmpty()) {
        if (m_initialized) {
            saveConfig();
        }

        m_group = group;

        if (!m_name.isEmpty()) {
            loadConfig();
            m_initialized = true;
        }

        Q_EMIT groupChanged();
    }
}

QVariant ConfigManager::updateValue(const QString &key, const QVariant &input)
{
    if (!m_initialized) {
        return QQmlPropertyMap::updateValue(key, input);
    }

    QVariant oldValue = value(key);

    if (oldValue != input) {
        QQmlPropertyMap::updateValue(key, input);

        if (!m_loading) {
            saveConfig();
        }
    }

    return input;
}

void ConfigManager::loadConfig()
{
    if (m_loading || m_name.isEmpty() || m_group.isEmpty())
        return;

    m_loading = true;

    QString filePath = getConfigFilePath();
    QFile file(filePath);

    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();

        if (!doc.isNull() && doc.isObject()) {
            QJsonObject configObj = doc.object();

            QStringList keys = this->keys();
            for (const QString &key : std::as_const(keys)) {
                if (key != "name" && key != "group") {
                    this->clear(key);
                }
            }

            for (auto it = configObj.begin(); it != configObj.end(); ++it) {
                QString key = it.key();
                QVariant value = it.value().toVariant();
                this->insert(key, value);
            }

            Q_EMIT configReloaded();
        }
    } else {
        QStringList keys = this->keys();
        for (const QString &key : std::as_const(keys)) {
            if (key != "name" && key != "group") {
                this->clear(key);
            }
        }

        Q_EMIT configReloaded();
    }

    m_loading = false;
}

void ConfigManager::saveConfig()
{
    if (m_loading || !m_initialized || m_name.isEmpty() || m_group.isEmpty())
        return;

    ensureConfigDirExists();
    QString filePath = getConfigFilePath();
    QFile file(filePath);

    QJsonObject configObj;

    QStringList keys = this->keys();
    for (const QString &key : std::as_const(keys)) {
        if (key == "name" || key == "group") continue;

        QVariant value = this->value(key);
        configObj[key] = QJsonValue::fromVariant(value);
    }

    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(configObj);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }
}

void ConfigManager::ensureConfigDirExists()
{
    QString configDir = getAppConfigDir();
    QDir dir(configDir);

    if (!dir.exists()) {
        dir.mkpath(".");
    }
}

QString ConfigManager::getConfigFilePath() const
{
    QString configDir = getAppConfigDir();
    return configDir + "/" + m_group + ".json";
}

QString ConfigManager::getAppConfigDir() const
{
    QString baseDir;

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    baseDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#else
    baseDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
#endif

    if (baseDir.isEmpty()) {
        baseDir = QDir::currentPath() + "/config";
    }

    return baseDir + "/" + m_name;
}
