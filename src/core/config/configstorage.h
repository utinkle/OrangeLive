#pragma once
#include <QString>
#include <QVariantMap>

// Storage interface. Implementations should be thread-safe for simultaneous save/load calls.
class ConfigStorage {
public:
    virtual ~ConfigStorage() = default;
    // save to path; return true on success. Implementations should use atomic write (QSaveFile) when writing files.
    virtual bool save(const QString &path, const QVariantMap &data, QString &err) = 0;
    // load from path; return empty map on failure and set err appropriately.
    virtual QVariantMap load(const QString &path, QString &err) = 0;
};
