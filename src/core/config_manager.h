#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QQmlPropertyMap>
#include <QString>
#include <QVariant>
#include <QQmlComponent>

class ConfigManager : public QQmlPropertyMap
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString group READ group WRITE setGroup NOTIFY groupChanged)
    QML_ELEMENT

public:
    explicit ConfigManager(QObject *parent = nullptr);
    ~ConfigManager();

    QString name() const;
    void setName(const QString &name);

    QString group() const;
    void setGroup(const QString &group);

protected:
    QVariant updateValue(const QString &key, const QVariant &input) override;

Q_SIGNALS:
    void nameChanged();
    void groupChanged();
    void configReloaded();

private:
    void loadConfig();
    void saveConfig();
    void ensureConfigDirExists();

    QString getConfigFilePath() const;
    QString getAppConfigDir() const;

private:
    QString m_name;
    QString m_group;
    bool m_loading;
    bool m_initialized;
};

#endif // CONFIGMANAGER_H
