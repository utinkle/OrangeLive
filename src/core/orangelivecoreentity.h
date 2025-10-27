#ifndef ORANGELIVECOREENTITY_H
#define ORANGELIVECOREENTITY_H

#include <QObject>
#include <QSharedPointer>

#include "interface/appstartupentityinterface.h"

class OLModuleLoader;
class OLModuleVersionControl;

class OrangeLiveCoreEntity : public QObject, public AppStartupEntityInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID AppStartupEntityInterface_iid FILE "entity.json")
    Q_INTERFACES(AppStartupEntityInterface)

public:
    explicit OrangeLiveCoreEntity(QObject *parent = nullptr);
    ~OrangeLiveCoreEntity();

    QUrl entityModulePath() const;
    void initialize(QQmlApplicationEngine *engine);
    void finishedLoading(QQmlApplicationEngine *engine);

private:
    void initializeModuleInformation();
    void installAndLoadDefaultModules();

private:
    QScopedPointer<OLModuleLoader> loader;
};

#endif // ORANGELIVECOREENTITY_H
