#ifndef OLMODULELOADER_H
#define OLMODULELOADER_H

#include <QObject>

#include "olmoduleinformation.h"

class QQuickItem;
class QQmlComponent;
class OLModuleVersionControl;

class OLModuleLoaderPrivate;
class OLModuleLoader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList allModules READ allModules NOTIFY allModulesChanged FINAL)
    Q_PROPERTY(QStringList loadedModules READ loadedModules NOTIFY loadedModulesChanged FINAL)
    Q_PROPERTY(QQmlComponent *moduleTemplate READ moduleTemplate WRITE setModuleTemplate NOTIFY moduleTemplateChanged FINAL)
    Q_PROPERTY(QQmlListProperty<QQuickItem> runingModuleItems READ runingModuleItems NOTIFY runingModuleItemsChanged FINAL)

public:
    explicit OLModuleLoader(QQmlEngine *engine, QObject *parent = nullptr);

    Q_INVOKABLE QList<OLModuleInformation> resolveModule(const QString &path);

    Q_INVOKABLE bool loadModule(const OLModuleInformation &information);
    Q_INVOKABLE void unloadModule(const OLModuleInformation &information);

    Q_INVOKABLE QList<OLModuleInformation> moduleDependencies(const OLModuleInformation &information) const;
    Q_INVOKABLE QList<OLModuleInformation> moduleDepents(const OLModuleInformation &information) const;

    Q_INVOKABLE OLModuleVersionControl *moduleVC(const QString &name);
    Q_INVOKABLE OLModuleInformation moduleInformation(const QString &name);

    QQmlComponent *moduleTemplate() const;
    void setModuleTemplate(QQmlComponent *moduleTemplate);

    QStringList allModules() const;
    QStringList loadedModules() const;

    QQmlListProperty<QQuickItem> runingModuleItems();

Q_SIGNALS:
    void moduleTemplateChanged();
    void allModulesChanged();
    void loadedModulesChanged();
    void runingModuleItemsChanged();

private:
    OLModuleLoaderPrivate *dd;
};

#endif // OLMODULELOADER_H
