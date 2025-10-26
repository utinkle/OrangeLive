#ifndef OLMODULEVERSIONCONTROL_H
#define OLMODULEVERSIONCONTROL_H

#include <QObject>
#include <QQmlListProperty>

#include "olmoduleinformation.h"

class OLModuleVersionControlPrivate;
class OLModuleVersionControl : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name CONSTANT FINAL)
    Q_PROPERTY(OLModuleInformation currentVersion READ currentVersion CONSTANT FINAL)
    Q_PROPERTY(QQmlListProperty<OLModuleInformation> allVersions READ allVersions CONSTANT FINAL)

public:
    explicit OLModuleVersionControl(const QString &name, QObject *parent = nullptr);
    ~OLModuleVersionControl();

    QString name() const;

    OLModuleInformation currentVersion() const;
    QQmlListProperty<OLModuleInformation> allVersions();

    void addVersion(const OLModuleInformation &version);
    void removeVersion(const OLModuleInformation &version);

private:
    QScopedPointer<OLModuleVersionControlPrivate> dd;
};

#endif // OLMODULEVERSIONCONTROL_H
