#ifndef OLHOMEPAGEPRELOAD_H
#define OLHOMEPAGEPRELOAD_H

#include <QObject>
#include <QQmlEngine>

#include "interface/appstartuppreloadinterface.h"

class OLHomePagePreload : public QObject, public AppStartupPreloadInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID AppStartupPreloadInterface_iid FILE "preload.json")
    Q_INTERFACES(AppStartupPreloadInterface)

public:
    explicit OLHomePagePreload(QObject *parent = nullptr);
    ~OLHomePagePreload();

    void aboutToPreload(QQmlApplicationEngine *engine);
    QUrl preloadModulePath() const;
};

#endif // OLHOMEPAGE_H
