#include "olmoduleloader.h"

#include "olmoduleversioncontrol.h"
#include "olmoduletemplate.h"

#include <QDir>
#include <QQmlContext>
#include <QQmlEngine>
#include <QUuid>
#include <QQmlComponent>
#include <QQmlProperty>

#include <appstartupinstance.h>
#include <items/appstartupmodulegroup.h>

#define TITLE_KEY "title"
#define PRODUCTOR_KEY "productor"
#define TITLE_ALIAS_KEY "titleAlias"
#define DEPENDS_KEY "depends"
#define MODULE_TYPE "moduleType"
#define AUTOLOAD_TYPE "autoLoad"
#define SURFACE_PROP "surface"

Q_GLOBAL_STATIC(OLModuleLoader *, _globalLoader)

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
    QPointer<QQuickItem> templateItem;

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
    QList<QQuickItem *> runningModuleItems;
    QQuickItem *moduleView;

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
        information.autoLoad = properties.value(AUTOLOAD_TYPE).toBool();

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
            qWarning() << "[OLModule] Need create module tempate before loading the module";
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

        templateItem->setParentItem(moduleView);
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

        if (auto item = findModuleSurfaceItem(templateItem)) {
            data->module->setSurfaceItem(item);
        }

        QObject::connect(data->module.data(), &AppStartupModuleGroup::loadedChanged, qq, [&, templateItem]() {
            data->loadStatus = OLModuleData::LOADED;
            data->templateItem = templateItem;

            auto vc = moduleVCHash.value(data->information.name);
            if (!vc.isNull()) {
                vc->setCurrentVersion(data->information);
            }

            runningModuleItems.append(templateItem);
            Q_EMIT qq->runingModuleItemsChanged();
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

        runningModuleItems.removeOne(module->templateItem);

        Q_EMIT qq->runingModuleItemsChanged();
        Q_EMIT qq->loadedModulesChanged();

        module->templateItem->deleteLater();
        module->templateItem.clear();

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

    QList<AppStartupModuleInformation> resolveInformationByDir(const QString &dirpath)
    {
        QDir dir(dirpath);
        QList<AppStartupModuleInformation> result;
        const auto &entryList = dir.entryInfoList(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

        for (const auto &entry : entryList) {
            auto splitList = entry.fileName().split(QChar('_'));
            if (splitList.isEmpty())
                continue;

            if (splitList.first() != QLatin1String("libplugins"))
                continue;

            splitList.takeFirst();
            if (splitList.isEmpty())
                continue;

            if (splitList.first() != QLatin1String("OLPlugin"))
                continue;

            result << std::move(AppStartupModuleInformation(entry.absoluteFilePath()));
        }

        return result;
    }
};

OLModuleLoader::OLModuleLoader(QQmlEngine *engine, QObject *parent)
    : QObject{parent}
    , dd(new OLModuleLoaderPrivate(this))
{
    dd->qml_engine = engine;
}

OLModuleLoader::~OLModuleLoader()
{

}

OLModuleLoader *OLModuleLoader::create(QQmlEngine *engine, QObject *parent)
{
    if (!_globalLoader.exists()) {
        *_globalLoader = new OLModuleLoader(engine, parent);
    }

    return *_globalLoader;
}

OLModuleLoader *OLModuleLoader::instance()
{
    return *_globalLoader;
}

QList<OLModuleInformation> OLModuleLoader::resolveModule(const QString &path)
{
    QList<OLModuleInformation> result;
    const auto &moduleInfos = dd->resolveInformationByDir(path);

    QSet<QPair<int, int>> processedPairs;

    for (int i = 0; i < moduleInfos.size(); ++i) {
        for (int j = i + 1; j < moduleInfos.size(); ++j) {
            if (processedPairs.contains({i, j}) || processedPairs.contains({j, i}))
                continue;

            if (moduleInfos[i].startModule() == moduleInfos[j].startModule()
                || moduleInfos[i].appId() != moduleInfos[j].appId()
                || moduleInfos[i].descriptor() != moduleInfos[j].descriptor()
                || moduleInfos[i].version() != moduleInfos[j].version())
                continue;

            processedPairs.insert({i, j});

            AppStartupModuleInformation preload = moduleInfos[i], entity = moduleInfos[j];
            if (entity.startModule() == AppStartupModuleInformation::Preload) {
                preload = moduleInfos[j];
                entity = moduleInfos[i];
            }

            auto module = QSharedPointer<AppStartupModuleGroup>::create(std::make_pair(preload, entity));
            if (!module->isValid())
                continue;

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
            break;
        }
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

QQuickItem *OLModuleLoader::moduleView() const
{
    return dd->moduleView;
}

void OLModuleLoader::setModuleView(QQuickItem *moduleView)
{
    if (dd->moduleView == moduleView)
        return;

    dd->moduleView = moduleView;
    Q_EMIT moduleViewChanged();
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

QQmlListProperty<QQuickItem> OLModuleLoader::runingModuleItems()
{
    return QQmlListProperty<QQuickItem>(this, &dd->runningModuleItems);
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

class OLModulePagesAttachedPrivate {
public:
    OLModulePagesAttachedPrivate(OLModulePagesAttached *qq, QObject *attachedObject)
        : qq(qq)
        , _attachedObject(attachedObject)
    {
        if (!_attachedObject)
            return;

        QQuickItem *rootTemplate = findRootTemplateItem(_attachedObject);
        if (rootTemplate)
            bindToSwipeAttached(rootTemplate);
    }

    QQuickItem *findRootTemplateItem(QObject *obj)
    {
        auto p = obj ? qobject_cast<QQuickItem *>(obj->parent()) : nullptr;
        while (p) {
            if (QString(p->metaObject()->className()).startsWith(QLatin1String("OLTemplate"))) {
                if (QQuickItem *item = qobject_cast<QQuickItem *>(p))
                    return item;
            }

            p = p->parentItem();
        }

        return nullptr;
    }

    void bindToSwipeAttached(QObject *rootTemplate)
    {
        if (!rootTemplate)
            return;

        OLModuleLoader *loader = OLModuleLoader::instance();
        if (!loader)
            return;

        QQuickItem *moduleView = loader->moduleView();
        if (!moduleView)
            return;

        auto func = qmlAttachedPropertiesFunction(moduleView, QMetaType::fromName("QQuickSwipeView*").metaObject());
        QObject *attached = qmlAttachedPropertiesObject(rootTemplate, func, true);
        if (!attached)
            return;

        _swipeViewAttached = attached;
        _swipeView = moduleView;

        const char *_attachedSignals[] = {
            "indexChanged()",
            "isCurrentItemChanged()",
            "isNextItemChanged()",
            "isPreviousItemChanged()"
        };

        const char *_viewSignals[] = {
            "layoutStateChanged()"
        };

        const char *slotSignature = "updateFromAttached()";

        const QMetaObject *receiverMeta = qq->metaObject();
        int recvMethodIndex = receiverMeta->indexOfMethod(QMetaObject::normalizedSignature(slotSignature));
        QMetaMethod receiverMethod;
        if (recvMethodIndex != -1)
            receiverMethod = receiverMeta->method(recvMethodIndex);

        const QMetaObject *attachedSenderMeta = _swipeViewAttached->metaObject();

        for (auto sigName : _attachedSignals) {
            int sigIndex = attachedSenderMeta->indexOfSignal(QMetaObject::normalizedSignature(sigName));
            if (sigIndex == -1) {
                continue;
            }

            QMetaMethod signalMethod = attachedSenderMeta->method(sigIndex);

            bool connected = false;
            if (recvMethodIndex != -1) {
                QMetaObject::Connection c = QObject::connect(_swipeViewAttached, signalMethod, qq, receiverMethod);
                connected = (c);
            }
            if (!connected) {
                QObject::connect(_swipeViewAttached, sigName, qq, slotSignature);
            }
        }

        const QMetaObject *viewSenderMeta = _swipeView->metaObject();

        for (auto sigName : _viewSignals) {
            int sigIndex = viewSenderMeta->indexOfSignal(QMetaObject::normalizedSignature(sigName));
            if (sigIndex == -1) {
                continue;
            }

            QMetaMethod signalMethod = viewSenderMeta->method(sigIndex);

            bool connected = false;
            if (recvMethodIndex != -1) {
                QMetaObject::Connection c = QObject::connect(_swipeView, signalMethod, qq, receiverMethod);
                connected = (c);
            }
            if (!connected) {
                QObject::connect(_swipeView, sigName, qq, slotSignature);
            }
        }

        QMetaObject::invokeMethod(qq, "updateFromAttached", Qt::QueuedConnection);
    }

    OLModulePagesAttached *qq;

    QPointer<QObject> _attachedObject;
    QPointer<QObject> _swipeViewAttached;
    QPointer<QObject> _swipeView;

    bool _isCurrentItem = false;
    bool _isNextItem = false;
    bool _isPreviousItem = false;
    bool _active = false;
    int _viewLayout = -1;
    int _index = -1;
};

OLModulePagesAttached::OLModulePagesAttached(QObject *parent)
    : QObject(parent)
    , dd(new OLModulePagesAttachedPrivate(this, parent))
{
}

OLModulePagesAttached::~OLModulePagesAttached()
{

}

int OLModulePagesAttached::index() const
{
    return dd->_index;
}

bool OLModulePagesAttached::isCurrentItem() const
{
    return dd->_isCurrentItem;
}

bool OLModulePagesAttached::isNextItem() const
{
    return dd->_isNextItem;
}

bool OLModulePagesAttached::isPreviousItem() const
{
    return dd->_isPreviousItem;
}

bool OLModulePagesAttached::isActive() const
{
    return dd->_active;
}

OLModulePagesAttached *OLModulePagesAttached::qmlAttachedProperties(QObject *object)
{
    return new OLModulePagesAttached(object);
}

void OLModulePagesAttached::updateFromAttached()
{
    if (!dd->_swipeViewAttached)
        return;

    QQmlProperty pIndex(dd->_swipeViewAttached, "index");
    QQmlProperty pCurrent(dd->_swipeViewAttached, "isCurrentItem");
    QQmlProperty pNext(dd->_swipeViewAttached, "isNextItem");
    QQmlProperty pPrev(dd->_swipeViewAttached, "isPreviousItem");
    QQmlProperty pLayout(dd->_swipeView, "layoutState");

    int newIndex = pIndex.read().toInt();
    bool newCur = pCurrent.read().toBool();
    bool newNext = pNext.read().toBool();
    bool newPrev = pPrev.read().toBool();
    bool newLayout = pLayout.read().toInt();

    if (newIndex != dd->_index) {
        dd->_index = newIndex;
        Q_EMIT indexChanged();
    }
    if (newCur != dd->_isCurrentItem) {
        dd->_isCurrentItem = newCur;
        Q_EMIT isCurrentItemChanged();
    }
    if (newNext != dd->_isNextItem) {
        dd->_isNextItem = newNext;
        Q_EMIT isNextItemChanged();
    }
    if (newPrev != dd->_isPreviousItem) {
        dd->_isPreviousItem = newPrev;
        Q_EMIT isPreviousItemChanged();
    }
    if (newLayout != dd->_viewLayout) {
        dd->_viewLayout = newLayout;
    }
    bool isActive = (dd->_viewLayout == 0) && (dd->_isCurrentItem);
    if (dd->_active != isActive) {
        dd->_active = isActive;
        Q_EMIT isActiveChanged();
    }
}
