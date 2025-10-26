#include "orangelivecoreentity.h"
#include "olmoduleloader.h"
#include "olmoduleversioncontrol.h"

#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QStandardPaths>
#include <QLibraryInfo>
#include <QFileInfo>
#include <QDir>

#include <appstartupinstance.h>
#include <items/appstartupmodulegroup.h>


OrangeLiveCoreEntity::OrangeLiveCoreEntity(QObject *parent)
    : QObject(parent)
    , loader(new OLModuleLoader(this))
{

}

OrangeLiveCoreEntity::~OrangeLiveCoreEntity()
{

}

QUrl OrangeLiveCoreEntity::entityModulePath() const
{
    return QUrl("qrc:///qml/entity/MainWindow.qml");
}

void OrangeLiveCoreEntity::initialize(QQmlApplicationEngine *engine)
{
    engine->rootContext()->setContextProperty("OLModuleLoader", loader.get());

    initializeModuleInformation();
}

void OrangeLiveCoreEntity::finishedLoading(QQmlApplicationEngine *engine)
{
    QNativeInterface::QAndroidApplication::hideSplashScreen();
}

void OrangeLiveCoreEntity::initializeModuleInformation()
{
    const QString &defaultModulePath = QLibraryInfo::path(QLibraryInfo::LibrariesPath)
                + QDir::separator() + ORANGE_ARCH_STRING + QDir::separator() + QLatin1String("modules");
    AppStartupInstance::instance()->addModulePath(defaultModulePath);
    QDir defaultMouduleDir(defaultModulePath);
    const auto &moduleEntryInfo = defaultMouduleDir.entryInfoList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);

    for (const auto &moduleInfo : moduleEntryInfo) {
        auto modules = loader->resolveModule(moduleInfo.absoluteFilePath());
        for (auto module : modules) {
            QSharedPointer<OLModuleVersionControl> vc;
            if (moduleVCHash.contains(module.name)) {
                vc = moduleVCHash.value(module.name);
            } else {
                vc = QSharedPointer<OLModuleVersionControl>::create(module.name, this);
                moduleVCHash.insert(module.name, vc);
            }

            vc->addVersion(module);

            // 接着处理插件的依赖关系
        }
    }
}
