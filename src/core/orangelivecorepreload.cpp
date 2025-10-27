#include "orangelivecorepreload.h"

#include <QQmlApplicationEngine>
#include <QLibraryInfo>
#include <QDir>

OrangeLiveCorePreload::OrangeLiveCorePreload(QObject *parent)
    : QObject(parent)
{

}

OrangeLiveCorePreload::~OrangeLiveCorePreload()
{

}

void OrangeLiveCorePreload::aboutToPreload(QQmlApplicationEngine *engine)
{
    engine->addPluginPath(QLibraryInfo::path(QLibraryInfo::DataPath) + QDir::separator() + ORANGE_ARCH_STRING);
}

QUrl OrangeLiveCorePreload::preloadModulePath() const
{
    return QUrl("qrc:///OL.Core/qml/preload/PreloadWindow.qml");
}
