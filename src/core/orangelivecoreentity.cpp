#include "orangelivecoreentity.h"
#include "olmoduleloader.h"
#include "olmoduletemplate.h"
#include "fileio.h"

#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QStandardPaths>
#include <QLibraryInfo>
#include <QFileInfo>
#include <QDir>

#include <appstartupinstance.h>
#include <items/appstartupmodulegroup.h>

#define HOME_PAGE_NAME "OLHomePage"


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
    if (loader == nullptr) {
        loader = OLModuleLoader::create(engine, this);
    }

    qmlRegisterType<OLModuleTemplate>("OL.Core", 1, 0, "OLModuleTemplate");
    qmlRegisterType<FileIO>("OL.Core", 1, 0, "FileIO");
    qmlRegisterSingletonType<OLModuleLoader>("OL.Core", 1, 0, "OLModuleLoader",
                                             [&](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject *{
        return loader;
    });
    qmlRegisterUncreatableType<OLModulePagesAttached>("OL.Core", 1, 0, "OLModulePage",
        "Can't create OLModulePage attached type"
    );

    initializeModuleInformation();
}

void OrangeLiveCoreEntity::finishedLoading(QQmlApplicationEngine *engine)
{
    QNativeInterface::QAndroidApplication::hideSplashScreen();
    installAndLoadDefaultModules();
}

void OrangeLiveCoreEntity::initializeModuleInformation()
{
    const QString &defaultModulePath = QLibraryInfo::path(QLibraryInfo::DataPath)
                + QDir::separator() + ORANGE_ARCH_STRING;
    AppStartupInstance::instance()->addModulePath(defaultModulePath);
    QDir defaultMouduleDir(defaultModulePath);
    loader->resolveModule(defaultMouduleDir.absolutePath());
}

void OrangeLiveCoreEntity::installAndLoadDefaultModules()
{
    for (const auto &moduleName : loader->allModules()) {
        auto information = loader->moduleInformation(moduleName);
        if (information.moduleType == OLModuleInformation::Component
            || information.name == HOME_PAGE_NAME) {
            // 组件和首页需要默认加载
            loader->loadModule(information);
        } else if (information.autoLoad) {
            loader->loadModule(information);
        }
    }
}
