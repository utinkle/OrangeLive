#include "config.h"
#include "configstorage.h"

#include <QtConcurrent>
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QDebug>
#include <QGuiApplication>

std::shared_ptr<ConfigStorage> createJsonStorage();
std::shared_ptr<ConfigStorage> createBinaryStorage();
std::shared_ptr<ConfigStorage> createSqliteStorage();

std::shared_ptr<ConfigStorage> createStorage(Config::StorageBackend backend)
{
    switch (backend) {
    case Config::StorageBackend::Json:
        return createJsonStorage();
    case Config::StorageBackend::Binary:
        return createBinaryStorage();
    case Config::StorageBackend::SQLite:
        return createSqliteStorage();
    case Config::StorageBackend::Custom:
    default:
        return createJsonStorage();
    }
}

Config::Config(QObject *parent)
    : ConfigObject(parent)
{
    {
        std::lock_guard<QMutex> lock(m_storageMutex);
        m_storage = createStorage(m_backend);
    }

    m_debounceTimer.setSingleShot(true);
    m_debounceTimer.setInterval(400);
    connect(&m_debounceTimer, &QTimer::timeout, this, &Config::onDebounceTimeout);

    connect(this, &ConfigObject::changed, this, &Config::onChildChanged, Qt::UniqueConnection);
    connect(qGuiApp, &QCoreApplication::aboutToQuit, this, [this]() {
        if (m_watcher.isRunning()) {
            m_watcher.waitForFinished();
        }
    });
}

Config::~Config()
{
    if (m_watcher.isRunning()) {
        m_watcher.waitForFinished();
    }
}

QString Config::name() const
{
    return m_name;
}

void Config::setName(const QString &name)
{
    if (m_name == name) {
        return;
    }
    m_name = name;
    Q_EMIT nameChanged();
}

QString Config::group() const
{
    return m_group;
}

void Config::setGroup(const QString &group)
{
    if (m_group == group) {
        return;
    }
    m_group = group;
    Q_EMIT groupChanged();
}

Config::StorageBackend Config::backend() const
{
    return m_backend;
}

void Config::setBackend(Config::StorageBackend backend)
{
    if (m_backend == backend) {
        return;
    }

    m_backend = backend;
    std::lock_guard<QMutex> lock(m_storageMutex);
    m_storage = createStorage(m_backend);
    Q_EMIT backendChanged();
}

void Config::load()
{
    std::shared_ptr<ConfigStorage> storage;
    {
        std::lock_guard<QMutex> lock(m_storageMutex);
        storage = m_storage;
    }

    if (!storage) {
        return;
    }

    QString path;
    if (m_backend == StorageBackend::SQLite) {
        path = QString("%1|%2").arg(m_name, m_group);
    } else {
        path = filePath();
    }

    auto feture = QtConcurrent::run([storage, path](QPromise<QVariantMap> &promise) {
        QString error;
        const QVariantMap loaded = storage->load(path, error);
        if (!error.isEmpty()) {
            qWarning() << "Load error:" << error;
        }

        promise.addResult(loaded);
    });

    auto loaded = feture.result();
    fromVariantMap(loaded);
}

void Config::save()
{
    scheduleSave();
}

QVariantMap Config::toSavedVariantMap() const
{
    QVariantMap full = toVariantMap();
    return full;
}

void Config::onChildChanged()
{
    scheduleSave();
    Q_EMIT updated();
}

void Config::scheduleSave()
{
    if (m_debounceTimer.isActive())
        return;

    m_debounceTimer.start();
}

void Config::onDebounceTimeout()
{
    saveNowAsync(toSavedVariantMap());
}

void Config::componentComplete()
{
    ConfigObject::componentComplete();
    loadFromInit();
}

QString Config::filePath() const
{
    QString base = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (base.isEmpty()) {
        base = QDir::homePath();
    }

    QDir baseDir(base);
    const QString dirPath = baseDir.filePath(m_name.isEmpty() ? "config" : m_name);
    QDir configDir(dirPath);
    if (!configDir.exists()) {
        configDir.mkpath(".");
    }

    QString fileName = m_group.isEmpty() ? "default" : m_group;
    return configDir.filePath(fileName);
}

void Config::saveNowAsync(const QVariantMap &map)
{
    std::shared_ptr<ConfigStorage> storage;
    {
        std::lock_guard<QMutex> lock(m_storageMutex);
        storage = m_storage;
    }

    if (!storage) {
        qWarning() << "No storage backend available";
        return;
    }

    const QString path = (m_backend == StorageBackend::SQLite)
                             ? QString("%1|%2").arg(m_name, m_group)
                             : filePath();

    auto storageCopy = storage;
    QFuture<void> future = QtConcurrent::run([storageCopy, path, map]() {
        QString error;
        if (!storageCopy->save(path, map, error)) {
            qWarning() << "Async save failed:" << error;
        }
    });

    m_watcher.setFuture(future);
}

void Config::loadImpl(bool async)
{
    std::shared_ptr<ConfigStorage> storage;
    {
        std::lock_guard<QMutex> lock(m_storageMutex);
        storage = m_storage;
    }

    if (!storage) {
        return;
    }

    QString path;
    if (m_backend == StorageBackend::SQLite) {
        path = QString("%1|%2").arg(m_name, m_group);
    } else {
        path = filePath();
    }

    if (async) {
        loadAsync(storage, path);
    } else {
        loadSync(storage, path);
    }
}

void Config::loadAsync(const std::shared_ptr<ConfigStorage> &storage, const QString &path)
{
    auto feture = QtConcurrent::run([this, storage, path](QPromise<QVariantMap> &promise) {
        QString error;
        const QVariantMap loaded = storage->load(path, error);
        if (!error.isEmpty()) {
            qWarning() << "Load error:" << error;
        }

        QMetaObject::invokeMethod(this, [this, loaded](){
            this->fromVariantMap(loaded);
        }, Qt::QueuedConnection);
    });
}

void Config::loadSync(const std::shared_ptr<ConfigStorage> &storage, const QString &path)
{
    auto feture = QtConcurrent::run([this, storage, path](QPromise<QVariantMap> &promise) {
        QString error;
        const QVariantMap loaded = storage->load(path, error);
        if (!error.isEmpty()) {
            qWarning() << "Load error:" << error;
        }

        promise.addResult(loaded);
    });

    auto loaded = feture.result();
    fromVariantMap(loaded);
}

void Config::loadFromInit()
{
    loadImpl(false);
    m_initialized = true;
    Q_EMIT updated();
}

QVariantMap Config::childrenProperties() const
{
    if (!m_initialized)
        return {};

    return toSavedVariantMap();
}
