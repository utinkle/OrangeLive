#ifndef OLHELLOWORLDENTITY_H
#define OLHELLOWORLDENTITY_H

#include <QObject>
#include <QSharedPointer>

#include "interface/appstartupentityinterface.h"

class OLHelloWorldEntity : public QObject, public AppStartupEntityInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID AppStartupEntityInterface_iid FILE "entity.json")
    Q_INTERFACES(AppStartupEntityInterface)

public:
    explicit OLHelloWorldEntity(QObject *parent = nullptr);
    ~OLHelloWorldEntity();

    QUrl entityModulePath() const;
};

#endif // OLHELLOWORLDENTITY_H
