#include "olhomepagepreload.h"

OLHomePagePreload::OLHomePagePreload(QObject *parent)
    : QObject(parent)
{

}

OLHomePagePreload::~OLHomePagePreload()
{

}

void OLHomePagePreload::aboutToPreload(QQmlApplicationEngine *engine)
{
}

QUrl OLHomePagePreload::preloadModulePath() const
{
    return QUrl("qrc:///OL.HomePage/qml/preload/PreloadWindow.qml");
}
