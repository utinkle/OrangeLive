#include "olmoduleloader.h"

#include <QDir>
#include <QUuid>

#include <appstartupinstance.h>
#include <items/appstartupmodulegroup.h>

#define TITLE_KEY "title"
#define PRODUCTOR_KEY "productor"
#define TITLE_ALIAS_KEY "titleAlias"
#define DEPENDS_KEY "depends"

class OLModuleLoaderPrivate {
public:
    OLModuleLoaderPrivate(OLModuleLoader *qq)
        : qq(qq)
    {}

    OLModuleLoader qq;
    QMap<QString, QSharedPointer<AppStartupModuleGroup>> moduleMaps;
};


OLModuleLoader::OLModuleLoader(QObject *parent)
    : QObject{parent}
{}

QList<OLModuleInformation> OLModuleLoader::resolveModule(const QString &path)
{
    QList<OLModuleInformation> result;
    const auto &modules = AppStartupModuleGroup::loadFromPath(path);
    if (modules.isEmpty())
        return result;

    for (auto module : modules) {
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
        information.workPath = path;

        dd->moduleMaps.insert(information.id, module);
        result << information;
    }

    return result;
}

bool OLModuleLoader::loadModule(const OLModuleInformation &information, QQuickItem *container)
{
    auto module = dd->moduleMaps.value(information.id);
    if (module.isNull()) {
        qWarning() << "[OLModule] load module is empty, id: " << information.id;
        return false;
    }

    if (container != nullptr)
        module->setSurfaceItem(container);

    return AppStartupInstance::instance()->load(module);
}

void OLModuleLoader::unloadModule(const OLModuleInformation &information)
{
    auto module = dd->moduleMaps.value(information.id);
    if (module.isNull()) {
        qWarning() << "[OLModule] unload module is empty, id: " << information.id;
        return;
    }

    return AppStartupInstance::instance()->unload(module);
}


