#ifndef OLHOMEPAGENTITY_H
#define OLHOMEPAGENTITY_H

#include <QObject>
#include <QSharedPointer>

#include "interface/appstartupentityinterface.h"

class OLHomePageEntity : public QObject, public AppStartupEntityInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID AppStartupEntityInterface_iid FILE "entity.json")
    Q_INTERFACES(AppStartupEntityInterface)

public:
    explicit OLHomePageEntity(QObject *parent = nullptr);
    ~OLHomePageEntity();

    QUrl entityModulePath() const;
};

#endif // OLHOMEPAGENTITY_H
