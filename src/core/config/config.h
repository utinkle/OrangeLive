#pragma once

#include "configobject.h"
#include "configstorage.h"

#include <QTimer>
#include <QFutureWatcher>
#include <QMutex>
#include <memory>
#include <QDateTime>

class Config : public ConfigObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
    Q_PROPERTY(QString group READ group WRITE setGroup NOTIFY groupChanged FINAL)
    Q_PROPERTY(StorageBackend backend READ backend WRITE setBackend NOTIFY backendChanged FINAL)
    Q_PROPERTY(QVariantMap childrenProperties READ childrenProperties NOTIFY updated FINAL)

public:
    enum class StorageBackend {
        Json,
        Binary,
        SQLite,
        Custom
    };
    Q_ENUM(StorageBackend)

    explicit Config(QObject *parent = nullptr);
    ~Config() override;

    QString name() const;
    void setName(const QString &name);

    QString group() const;
    void setGroup(const QString &group);

    StorageBackend backend() const;
    void setBackend(StorageBackend backend);

    QVariantMap childrenProperties() const;

    Q_INVOKABLE void load();
    Q_INVOKABLE void save();

Q_SIGNALS:
    void nameChanged();
    void groupChanged();
    void backendChanged();
    void updated();

private Q_SLOTS:
    void onChildChanged();
    void onDebounceTimeout();

private:
    void componentComplete() override;
    QVariantMap toSavedVariantMap() const;
    QString filePath() const;

    void scheduleSave();
    void saveNowAsync(const QVariantMap &map);

    void loadImpl(bool async);
    void loadAsync(const std::shared_ptr<ConfigStorage> &storage, const QString &path);
    void loadSync(const std::shared_ptr<ConfigStorage> &storage, const QString &path);
    void loadFromInit();

    bool m_initialized = false;
    QString m_name;
    QString m_group;
    StorageBackend m_backend = StorageBackend::Json;

    mutable QMutex m_storageMutex;
    std::shared_ptr<ConfigStorage> m_storage;

    QTimer m_debounceTimer;
    QFutureWatcher<void> m_watcher;
    QMutex m_saveMutex;
};
