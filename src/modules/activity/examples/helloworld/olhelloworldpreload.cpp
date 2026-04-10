#include "olhelloworldpreload.h"

OLHelloWorldPreload::OLHelloWorldPreload(QObject *parent)
    : QObject(parent)
{

}

OLHelloWorldPreload::~OLHelloWorldPreload()
{

}

void OLHelloWorldPreload::aboutToPreload(QQmlApplicationEngine *engine)
{
}

QUrl OLHelloWorldPreload::preloadModulePath() const
{
    return QUrl("qrc:///OL.HelloWorld/qml/preload/PreloadWindow.qml");
}
