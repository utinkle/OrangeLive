#include "olhomepageentity.h"
#include "impl/androidplaybackhelper.h"
#include "impl/clipboardhelper.h"
#include "impl/playlistservice.h"

#include <QtQml>

OLHomePageEntity::OLHomePageEntity(QObject *parent)
    : QObject(parent)
{

}

OLHomePageEntity::~OLHomePageEntity()
{

}

QUrl OLHomePageEntity::entityModulePath() const
{
    return QUrl("qrc:///OL.HomePage/qml/entity/HPMainWindow.qml");
}

void OLHomePageEntity::initialize(QQmlApplicationEngine *engine)
{
    qmlRegisterType<AndroidPlaybackHelper>("OL.HomePage", 1, 0, "AndroidPlaybackHelper");
    qmlRegisterType<ClipboardHelper>("OL.HomePage", 1, 0, "ClipboardHelper");
    qmlRegisterType<PlaylistService>("OL.HomePage", 1, 0, "PlaylistService");
}
