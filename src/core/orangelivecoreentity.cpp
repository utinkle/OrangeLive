#include "orangelivecoreentity.h"
#include "olmoduleloader.h"
#include "olmoduletemplate.h"

#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QStandardPaths>
#include <QLibraryInfo>
#include <QFileInfo>
#include <QDir>

#include <appstartupinstance.h>
#include <items/appstartupmodulegroup.h>

#define HOME_PAGE_NAME "HomePage"


OrangeLiveCoreEntity::OrangeLiveCoreEntity(QObject *parent)
    : QObject(parent)
    , loader(nullptr)
{

}

OrangeLiveCoreEntity::~OrangeLiveCoreEntity()
{

}

QUrl OrangeLiveCoreEntity::entityModulePath() const
{
    return QUrl("qrc:///OL.Core/qml/entity/MainWindow.qml");
}

void OrangeLiveCoreEntity::initialize(QQmlApplicationEngine *engine)
{
    if (loader.isNull()) {
        loader.reset(new OLModuleLoader(engine, this));
    }

    qmlRegisterType<OLModuleTemplate>("OL.Core", 1, 0, "OLModuleTemplate");
    qmlRegisterSingletonType<OLModuleLoader>("OL.Core", 1, 0, "OLModuleLoader",
                                             [&](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject *{
        return loader.get();
    });

    initializeModuleInformation();
    installAndLoadDefaultModules();
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
        loader->resolveModule(moduleInfo.absoluteFilePath());
    }
}

void OrangeLiveCoreEntity::installAndLoadDefaultModules()
{
    for (const auto &moduleName : loader->allModules()) {
        auto information = loader->moduleInformation(moduleName);
        if (information.moduleType == OLModuleInformation::Component
            || information.name == HOME_PAGE_NAME) {
            // 组件和首页需要默认加载
            loader->loadModule(information);
        }
    }
}
