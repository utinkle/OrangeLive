#ifndef ORANGELIVECOREPRELOAD_H
#define ORANGELIVECOREPRELOAD_H

#include <QObject>
#include <QQmlEngine>

#include "interface/appstartuppreloadinterface.h"

class OrangeLiveCorePreload : public QObject, public AppStartupPreloadInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID AppStartupPreloadInterface_iid FILE "preload.json")
    Q_INTERFACES(AppStartupPreloadInterface)

public:
    explicit OrangeLiveCorePreload(QObject *parent = nullptr);
    ~OrangeLiveCorePreload();

    void aboutToPreload(QQmlApplicationEngine *engine);
    QUrl preloadModulePath() const;
};

#endif // ORANGELIVECOREPRELOAD_H
