#include "olhomepageentity.h"

OLHomePageEntity::OLHomePageEntity(QObject *parent)
    : QObject(parent)
{

}

OLHomePageEntity::~OLHomePageEntity()
{

}

QUrl OLHomePageEntity::entityModulePath() const
{
    return QUrl("qrc:///OL.HomePage/qml/entity/MainWindow.qml");
}
