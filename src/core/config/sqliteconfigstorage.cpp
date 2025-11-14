#include "configstorage.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QStandardPaths>

class SqliteStorageImpl : public ConfigStorage {
public:
    SqliteStorageImpl() {
        QString base = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

        if (base.isEmpty())
            base = QDir::homePath();

        QDir d(base);

        if (!d.exists())
            d.mkpath(".");

        m_dbPath = d.filePath("config_store.sqlite");
        m_db = QSqlDatabase::addDatabase("QSQLITE", QString("cfg_conn_%1").arg(reinterpret_cast<quintptr>(this)));
        m_db.setDatabaseName(m_dbPath);

        if (!m_db.open()) {
            m_ok = false;
            return;
        }

        QSqlQuery q(m_db);
        q.exec("CREATE TABLE IF NOT EXISTS configs (name TEXT, group_name TEXT, data TEXT, PRIMARY KEY(name, group_name))");
        m_ok = true;
    }

    ~SqliteStorageImpl()
    {
        if (m_db.isOpen()) m_db.close();
        QSqlDatabase::removeDatabase(m_db.connectionName());
    }

    bool save(const QString &pathOrKey, const QVariantMap &data, QString &err) override {
        if (!m_ok) {
            err = "sqlite not ok";
            return false;
        }

        // pathOrKey format: name|group
        QStringList parts = pathOrKey.split('|');
        if (parts.size() != 2) {
            err = "invalid key for sqlite";
            return false;
        }

        QString name = parts[0], group = parts[1];
        QJsonDocument doc = QJsonDocument::fromVariant(data);
        QString json = QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
        QSqlQuery q(m_db);
        q.prepare("INSERT OR REPLACE INTO configs(name, group_name, data) VALUES(:n,:g,:d)");
        q.bindValue(":n", name); q.bindValue(":g", group); q.bindValue(":d", json);

        if (!q.exec()) {
            err = q.lastError().text();
            return false;
        }

        return true;
    }

    QVariantMap load(const QString &pathOrKey, QString &err) override {
        QVariantMap empty;
        if (!m_ok) {
            err = "sqlite not ok";
            return empty;
        }

        QStringList parts = pathOrKey.split('|');
        if (parts.size() != 2) {
            err = "invalid key for sqlite";
            return empty;
        }

        QString name = parts[0], group = parts[1];
        QSqlQuery q(m_db);
        q.prepare("SELECT data FROM configs WHERE name=:n AND group_name=:g");
        q.bindValue(":n", name);
        q.bindValue(":g", group);

        if (!q.exec()) {
            err = q.lastError().text();
            return empty;
        }

        if (q.next()) {
            QString json = q.value(0).toString();
            QJsonParseError pe;
            QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &pe);
            if (pe.error != QJsonParseError::NoError) {
                err = pe.errorString();
                return empty;
            }

            return doc.toVariant().toMap();
        }

        return empty;
    }

private:
    QString m_dbPath;
    QSqlDatabase m_db;
    bool m_ok = false;
};

std::shared_ptr<ConfigStorage> createSqliteStorage()
{
    return std::make_shared<SqliteStorageImpl>();
}
