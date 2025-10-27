#include "olmoduleloader.h"

#include "olmoduleversioncontrol.h"
#include "olmoduletemplate.h"

#include <QDir>
#include <QQmlContext>
#include <QQmlEngine>
#include <QUuid>
#include <QQmlComponent>

#include <appstartupinstance.h>
#include <items/appstartupmodulegroup.h>

#define TITLE_KEY "title"
#define PRODUCTOR_KEY "productor"
#define TITLE_ALIAS_KEY "titleAlias"
#define DEPENDS_KEY "depends"
#define MODULE_TYPE "moduleType"
#define SURFACE_PROP "surface"

struct OLModuleData {
    enum LoadStatus {
        UNLOADED,     // 未加载
        LOADING,      // 加载中
        LOADED,       // 已加载
        UNLOADING,    // 卸载中
        ERROR         // 错误状态
    };

    LoadStatus loadStatus;
    OLModuleInformation information;
    QSharedPointer<AppStartupModuleGroup> module;

    QSet<QString> dependents;
};

class OLModuleLoaderPrivate {
public:
    OLModuleLoaderPrivate(OLModuleLoader *qq)
        : qq(qq)
    {}

    OLModuleLoader *qq = nullptr;
    QQmlEngine *qml_engine = nullptr;
    QQmlComponent *moduleTemplate = nullptr;
    QHash<QString, QString> moduleNameIdHash;
    QHash<QString, QSharedPointer<OLModuleData>> moduleMaps;
    QHash<QString, QSharedPointer<OLModuleVersionControl>> moduleVCHash;

public:
    OLModuleInformation assignInformation(const QSharedPointer<AppStartupModuleGroup> &module, const QString &path)
    {
        OLModuleInformation information;
        information.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
        information.name = module->preload().descriptor();
        information.description = module->preload().description();
        information.version = module->preload().version();

        QVariantMap properties = module->preload().properties();
        information.title = properties.value(TITLE_KEY).toString();
        information.titleAlias = properties.value(TITLE_ALIAS_KEY).toString();
        information.productor = properties.value(PRODUCTOR_KEY).toString();
        information.depends = properties.value(DEPENDS_KEY).toStringList();
        information.moduleType = OLModuleInformation::ModuleType(properties.value(MODULE_TYPE).toInt());
        information.workPath = path;

        auto moduleData = QSharedPointer<OLModuleData>::create();
        moduleData->loadStatus = OLModuleData::UNLOADED;
        moduleData->module = module;
        moduleData->information = information;

        moduleNameIdHash.insert(information.name, information.id);
        moduleMaps.insert(information.id, moduleData);
        Q_EMIT qq->allModulesChanged();

        // 处理插件的依赖关系
        for (auto it = moduleMaps.cbegin(); it != moduleMaps.end(); ++it) {
            if (information.depends.contains((*it)->information.name)) {
                (*it)->dependents.insert(information.name);
            } else if ((*it)->information.depends.contains(information.name)) {
                moduleData->dependents.insert((*it)->information.name);
            }
        }

        return information;
    }

    bool collectDependencies(const QString& pluginId,
                             QList<QString>& dependencies) {
        QSet<QString> visited;
        return collectDependenciesRecursive(pluginId, dependencies, visited);
    }

    bool collectDependenciesRecursive(const QString &pluginId, QList<QString>& dependencies, QSet<QString>& visited) {
        if (!moduleMaps.contains(pluginId))
            return false;

        if (visited.contains(pluginId))
            return true;

        visited.insert(pluginId);

        const auto& deps = moduleMaps[pluginId]->information.depends;
        for (const auto& dep : deps) {
            const auto &depId = moduleNameIdHash.value(dep);
            if (depId.isEmpty())
                continue;

            if (!collectDependenciesRecursive(depId, dependencies, visited)) {
                return false;
            }
        }

        dependencies << pluginId;
        return true;
    }

    OLModuleTemplate *createModuleTemplate(const OLModuleInformation &information)
    {
        if (!this->moduleTemplate) {
            qWarning() << "[OLModule] create module tempate before loading the module";
            return nullptr;
        }

        QQmlEngine *engine = qml_engine != nullptr ? qml_engine : qmlEngine(qq->parent());
        if (!engine)
            return nullptr;

        QQmlContext *context = new QQmlContext(engine);

        OLModuleTemplate *templateItem = qobject_cast<OLModuleTemplate *>(this->moduleTemplate->beginCreate(context));
        if (!templateItem) {
            qWarning() << "[OLModule] module template create failed";
            return nullptr;
        }

        templateItem->setInformation(information);

        engine->setObjectOwnership(templateItem, QQmlEngine::CppOwnership);
        this->moduleTemplate->completeCreate();
        return templateItem;
    }

    QQuickItem *findModuleSurfaceItem(OLModuleTemplate *item)
    {
        if (!item)
            return nullptr;

        QVariant val = item->property(SURFACE_PROP);
        if (!val.isValid())
            return item;

        if (!val.canConvert<QQuickItem *>())
            return item;

        return val.value<QQuickItem *>();
    }

    bool loadPluginInternal(const QString& pluginId) {
        auto& data = moduleMaps[pluginId];
        if (data->loadStatus != OLModuleData::UNLOADED) {
            return data->loadStatus == OLModuleData::LOADED;
        }

        data->loadStatus = OLModuleData::LOADING;

        auto templateItem = createModuleTemplate(data->information);

        if (auto item = findModuleSurfaceItem(templateItem))
            data->module->setSurfaceItem(item);

        QObject::connect(data->module.data(), &AppStartupModuleGroup::loadedChanged, qq, [&]() {
            data->loadStatus = OLModuleData::LOADED;
            auto vc = moduleVCHash.value(data->information.name);
            if (!vc.isNull()) {
                vc->setCurrentVersion(data->information);
            }

            Q_EMIT qq->loadedModulesChanged();
        }, Qt::SingleShotConnection);

        QObject::connect(data->module.data(), &AppStartupModuleGroup::errorOccured, qq, [&](const QString &errorString) {
            data->loadStatus = OLModuleData::ERROR;
        }, Qt::SingleShotConnection);

        bool success = AppStartupInstance::instance()->load(data->module);
        if (!success)
            data->loadStatus = OLModuleData::ERROR;

        return success;
    }

    bool unloadPluginInternal(const QString &pluginId) {
        auto &module = moduleMaps[pluginId];
        if (module->loadStatus != OLModuleData::LOADED) {
            return true;
        }

        // 检查是否有活跃的依赖者
        if (!module->dependents.empty()) {
            // 找出仍然加载的依赖者
            QList<QString> activeDependents;
            for (const auto& dependent : module->dependents) {
                if (moduleMaps[dependent]->loadStatus == OLModuleData::LOADED ||
                    moduleMaps[dependent]->loadStatus == OLModuleData::LOADING) {
                    activeDependents.push_back(dependent);
                }
            }

            if (!activeDependents.empty()) {
                return false; // 有活跃依赖者，不能卸载
            }
        }

        module->loadStatus = OLModuleData::UNLOADING;
        AppStartupInstance::instance()->unload(module->module);
        module->loadStatus= OLModuleData::UNLOADED;
        return true;
    }

    bool collectAllDependenciesRecursive(const QString& pluginId,
                                         QSet<QString>& allDeps) const {
        auto it = moduleMaps.find(pluginId);
        if (it == moduleMaps.end()) {
            return false;
        }

        for (const auto& depName : (*it)->information.depends) {
            allDeps.insert(depName);
            if (!collectAllDependenciesRecursive(depName, allDeps)) {
                return false;
            }
        }

        return true;
    }

    bool collectAllDependentsRecursive(const QString& pluginId,
                                       QSet<QString>& allDependents) const {
        auto it = moduleMaps.find(pluginId);
        if (it == moduleMaps.end()) {
            return false;
        }

        for (const auto& dependentName : (*it)->dependents) {
            allDependents.insert(dependentName);
            if (!collectAllDependentsRecursive(dependentName, allDependents)) {
                return false;
            }
        }

        return true;
    }
};

OLModuleLoader::OLModuleLoader(QQmlEngine *engine, QObject *parent)
    : QObject{parent}
    , dd(new OLModuleLoaderPrivate(this))
{
    dd->qml_engine = engine;
}

QList<OLModuleInformation> OLModuleLoader::resolveModule(const QString &path)
{
    QList<OLModuleInformation> result;
    const auto &modules = AppStartupModuleGroup::loadFromPath(path);
    if (modules.isEmpty())
        return result;

    for (auto module : modules) {
        const OLModuleInformation &information = dd->assignInformation(module, path);
        result << information;

        QSharedPointer<OLModuleVersionControl> vc;
        if (dd->moduleVCHash.contains(information.name)) {
            vc = dd->moduleVCHash.value(information.name);
        } else {
            vc = QSharedPointer<OLModuleVersionControl>::create(information.name, this);
            dd->moduleVCHash.insert(information.name, vc);
        }

        vc->addVersion(information);
    }

    return result;
}

bool OLModuleLoader::loadModule(const OLModuleInformation &information)
{
    auto moduleData = dd->moduleMaps.value(information.id);
    if (moduleData.isNull()) {
        qWarning() << "[OLModule] load module is empty, id: " << information.id;
        return false;
    }

    QList<QString> dependencies;
    if (!dd->collectDependencies(information.id, dependencies)) {
        return false;
    }

    for (const auto& depId : dependencies) {
        if (!dd->loadPluginInternal(depId)) {
            return false;
        }
    }

    return dd->loadPluginInternal(information.id);
}

void OLModuleLoader::unloadModule(const OLModuleInformation &information)
{
    auto module = dd->moduleMaps.value(information.id);
    if (module.isNull()) {
        qWarning() << "[OLModule] unload module is empty, id: " << information.id;
        return;
    }

    dd->unloadPluginInternal(information.id);
}

QList<OLModuleInformation> OLModuleLoader::moduleDependencies(const OLModuleInformation &information) const
{
    QSet<QString> allDeps;
    QList<OLModuleInformation> result;

    if (dd->collectAllDependenciesRecursive(information.id, allDeps)) {
        for (auto it = allDeps.cbegin(); it != allDeps.cend(); ++it) {
            if (!dd->moduleNameIdHash.contains(*it))
                continue;

            result.append(dd->moduleMaps[dd->moduleNameIdHash[*it]]->information);
        }
    }

    return result;
}

QList<OLModuleInformation> OLModuleLoader::moduleDepents(const OLModuleInformation &information) const
{    
    QSet<QString> allDependents;
    QList<OLModuleInformation> result;

    if (dd->collectAllDependentsRecursive(information.id, allDependents)) {
        for (auto it = allDependents.cbegin(); it != allDependents.cend(); ++it) {
            if (!dd->moduleNameIdHash.contains(*it))
                continue;

            result.append(dd->moduleMaps[dd->moduleNameIdHash[*it]]->information);
        }
    }

    return result;
}

OLModuleVersionControl *OLModuleLoader::moduleVC(const QString &name)
{
    return dd->moduleVCHash.value(name).get();
}

QQmlComponent *OLModuleLoader::moduleTemplate() const
{
    return dd->moduleTemplate;
}

void OLModuleLoader::setModuleTemplate(QQmlComponent *moduleTemplate)
{
    if (dd->moduleTemplate == moduleTemplate)
        return;

    dd->moduleTemplate = moduleTemplate;
    Q_EMIT moduleTemplateChanged();
}

QStringList OLModuleLoader::allModules() const
{
    return dd->moduleNameIdHash.keys();
}

QStringList OLModuleLoader::loadedModules() const
{
    QStringList result;
    for (auto it = dd->moduleMaps.cbegin(); it != dd->moduleMaps.cend(); ++it) {
        if ((*it)->loadStatus == OLModuleData::LOADED) {
            result << (*it)->information.name;
        }
    }

    return result;
}

OLModuleInformation OLModuleLoader::moduleInformation(const QString &name)
{
    auto data = dd->moduleMaps.value(dd->moduleNameIdHash.value(name));
    if (!data) {
        qWarning() << "No module name: " << name << " found!";
        return {};
    }

    return data->information;
}
