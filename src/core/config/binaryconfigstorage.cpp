#include "configstorage.h"
#include <QSaveFile>
#include <QDataStream>
#include <QFile>

class BinaryStorageImpl : public ConfigStorage {
public:
    bool save(const QString &pathOrKey, const QVariantMap &data, QString &err) override {
        QSaveFile f(pathOrKey);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            err = "open failed";
            return false;
        }

        QDataStream out(&f);
        out << data;

        if (!f.commit()) {
            err = "commit failed";
            return false;
        }

        return true;
    }

    QVariantMap load(const QString &pathOrKey, QString &err) override {
        QFile f(pathOrKey);
        if (!f.exists())
            return {};

        if (!f.open(QIODevice::ReadOnly)) {
            err = "open failed";
            return {};
        }

        QDataStream in(&f);
        QVariantMap m;
        in >> m;
        return m;
    }
};

std::shared_ptr<ConfigStorage> createBinaryStorage()
{
    return std::make_shared<BinaryStorageImpl>();
}
