#ifndef OLHELLOWORLDPRELOAD_H
#define OLHELLOWORLDPRELOAD_H

#include <QObject>
#include <QQmlEngine>

#include "interface/appstartuppreloadinterface.h"

class OLHelloWorldPreload : public QObject, public AppStartupPreloadInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID AppStartupPreloadInterface_iid FILE "preload.json")
    Q_INTERFACES(AppStartupPreloadInterface)

public:
    explicit OLHelloWorldPreload(QObject *parent = nullptr);
    ~OLHelloWorldPreload();

    void aboutToPreload(QQmlApplicationEngine *engine);
    QUrl preloadModulePath() const;
};

#endif // OLHELLOWORLDPRELOAD_H
