#include "olhelloworldentity.h"

OLHelloWorldEntity::OLHelloWorldEntity(QObject *parent)
    : QObject(parent)
{

}

OLHelloWorldEntity::~OLHelloWorldEntity()
{

}

QUrl OLHelloWorldEntity::entityModulePath() const
{
    return QUrl("qrc:///OL.HelloWorld/qml/entity/MainWindow.qml");
}
