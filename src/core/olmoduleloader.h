#ifndef OLMODULELOADER_H
#define OLMODULELOADER_H

#include <QObject>

#include "olmoduleinformation.h"

class QQuickItem;

class OLModuleLoaderPrivate;
class OLModuleLoader : public QObject
{
    Q_OBJECT

public:
    explicit OLModuleLoader(QObject *parent = nullptr);

    QList<OLModuleInformation> resolveModule(const QString &path);

    bool loadModule(const OLModuleInformation &information, QQuickItem *container = nullptr);
    void unloadModule(const OLModuleInformation &information);

private:
    OLModuleLoaderPrivate *dd;
};

#endif // OLMODULELOADER_H
