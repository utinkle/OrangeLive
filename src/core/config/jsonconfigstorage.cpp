#include "configstorage.h"
#include <QSaveFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDir>

class JsonStorageImpl : public ConfigStorage {
public:
    bool save(const QString &pathOrKey, const QVariantMap &data, QString &err) override {
        QSaveFile f(pathOrKey);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            err = QString("open failed: %1").arg(pathOrKey);
            return false;
        }

        QJsonDocument doc = QJsonDocument::fromVariant(data);
        if (f.write(doc.toJson(QJsonDocument::Indented)) < 0) {
            err = "write failed";
            return false;
        }

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

        QJsonParseError pe;
        QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &pe);
        if (pe.error != QJsonParseError::NoError) {
            err = pe.errorString();
            return {};
        }

        return doc.toVariant().toMap();
    }
};

std::shared_ptr<ConfigStorage> createJsonStorage()
{
    return std::make_shared<JsonStorageImpl>();
}
