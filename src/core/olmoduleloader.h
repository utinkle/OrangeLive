#ifndef OLMODULELOADER_H
#define OLMODULELOADER_H

#include <QObject>
#include <QQuickItem>

#include "olmoduleinformation.h"

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
    Q_PROPERTY(QQuickItem *moduleView READ moduleView WRITE setModuleView NOTIFY moduleViewChanged FINAL)

public:
    explicit OLModuleLoader(QQmlEngine *engine, QObject *parent = nullptr);
    ~OLModuleLoader();

    static OLModuleLoader *create(QQmlEngine *engine, QObject *parent = nullptr);
    static OLModuleLoader *instance();

    Q_INVOKABLE QList<OLModuleInformation> resolveModule(const QString &path);

    Q_INVOKABLE bool loadModule(const OLModuleInformation &information);
    Q_INVOKABLE void unloadModule(const OLModuleInformation &information);

    Q_INVOKABLE QList<OLModuleInformation> moduleDependencies(const OLModuleInformation &information) const;
    Q_INVOKABLE QList<OLModuleInformation> moduleDepents(const OLModuleInformation &information) const;

    Q_INVOKABLE OLModuleVersionControl *moduleVC(const QString &name);
    Q_INVOKABLE OLModuleInformation moduleInformation(const QString &name);

    QQmlComponent *moduleTemplate() const;
    void setModuleTemplate(QQmlComponent *moduleTemplate);

    QQuickItem *moduleView() const;
    void setModuleView(QQuickItem *moduleView);

    QStringList allModules() const;
    QStringList loadedModules() const;

    QQmlListProperty<QQuickItem> runingModuleItems();

Q_SIGNALS:
    void moduleTemplateChanged();
    void allModulesChanged();
    void loadedModulesChanged();
    void runingModuleItemsChanged();
    void moduleViewChanged();

private:
    QScopedPointer<OLModuleLoaderPrivate> dd;
};

class OLModulePagesAttachedPrivate;

class OLModulePagesAttached : public QObject
{
    friend class OLModulePagesAttachedPrivate;

    Q_OBJECT
    Q_PROPERTY(int index READ index NOTIFY indexChanged FINAL)
    Q_PROPERTY(bool isCurrentItem READ isCurrentItem NOTIFY isCurrentItemChanged FINAL)
    Q_PROPERTY(bool isNextItem READ isNextItem NOTIFY isNextItemChanged FINAL)
    Q_PROPERTY(bool isPreviousItem READ isPreviousItem NOTIFY isPreviousItemChanged FINAL)
    Q_PROPERTY(bool isActive READ isActive NOTIFY isActiveChanged FINAL)

    QML_UNCREATABLE("OLModulePagesAttached Attached.")
    QML_ANONYMOUS

public:
    explicit OLModulePagesAttached(QObject *parent = nullptr);
    ~OLModulePagesAttached();

    int index() const;
    bool isCurrentItem() const;
    bool isNextItem() const;
    bool isPreviousItem() const;
    bool isActive() const;

    static OLModulePagesAttached *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    void indexChanged();
    void isCurrentItemChanged();
    void isNextItemChanged();
    void isPreviousItemChanged();
    void isActiveChanged();

private Q_SLOTS:
    void updateFromAttached();

private:
    QScopedPointer<OLModulePagesAttachedPrivate> dd;
};

QML_DECLARE_TYPEINFO(OLModulePagesAttached, QML_HAS_ATTACHED_PROPERTIES)

#endif // OLMODULELOADER_H
