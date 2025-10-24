#ifndef ORANGELIVECOREENTITY_H
#define ORANGELIVECOREENTITY_H

#include <QObject>

#include "interface/appstartupentityinterface.h"

class OrangeLiveCoreEntity : public QObject, public AppStartupEntityInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID AppStartupEntityInterface_iid FILE "entity.json")
    Q_INTERFACES(AppStartupEntityInterface)

public:
    explicit OrangeLiveCoreEntity(QObject *parent = nullptr);
    ~OrangeLiveCoreEntity();

    QUrl entityModulePath() const;
    void finishedLoading(QQmlApplicationEngine *engine);
};

#endif // ORANGELIVECOREENTITY_H
