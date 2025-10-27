#include "olmoduleversioncontrol.h"

class OLModuleVersionControlPrivate {
public:
    OLModuleVersionControlPrivate(OLModuleVersionControl *qq)
        :qq(qq) {}

    OLModuleVersionControl *qq;
    QString name;

    OLModuleInformation currentVersion;
    QList<OLModuleInformation *> versions;

    static bool versionGreaterThan(const QString &v1, const QString &v2);
};

bool OLModuleVersionControlPrivate::versionGreaterThan(const QString &v1, const QString &v2)
{
    const QStringList &v1Versions = v1.split('.');
    const QStringList &v2Versions = v1.split('.');

    if (v1Versions.isEmpty())
        return false;

    if (v2Versions.isEmpty())
        return true;

    int nums = qMin(v1Versions.length(), v2Versions.length());
    for (int i = 0; i < nums; ++i) {
        if (v1Versions[i] == v2Versions[1])
            continue;

        return v1Versions[i] > v2Versions[1];
    }

    // 相等
    return false;
}

OLModuleVersionControl::OLModuleVersionControl(const QString &name, QObject *parent)
    : QObject(parent)
    , dd(new OLModuleVersionControlPrivate(this))
{
    dd->name = name;
}

OLModuleVersionControl::~OLModuleVersionControl()
{

}

QString OLModuleVersionControl::name() const
{
    return dd->name;
}

OLModuleInformation OLModuleVersionControl::currentVersion() const
{
    return dd->currentVersion;
}

void OLModuleVersionControl::setCurrentVersion(const OLModuleInformation &information)
{
    if (information.name != dd->name) {
        qWarning() << "[OLModuleVC] module set current version failed!";
        return;
    }

    if (dd->currentVersion.version == information.version)
        return;

    dd->currentVersion = information;
    Q_EMIT currentVersionChanged();
}

OLModuleInformation OLModuleVersionControl::minVersion() const
{
    return dd->versions.isEmpty() ? OLModuleInformation() : *dd->versions.first();
}

OLModuleInformation OLModuleVersionControl::maxVersion() const
{
    return dd->versions.isEmpty() ? OLModuleInformation() : *dd->versions.last();
}

QQmlListProperty<OLModuleInformation> OLModuleVersionControl::allVersions()
{
    return QQmlListProperty<OLModuleInformation>(this, &dd->versions);
}

void OLModuleVersionControl::addVersion(const OLModuleInformation &version)
{
    if (version.name != dd->name) {
        qWarning() << "[OLModuleVC] add version failed, the version: " << version.name
                   << " is not the current vc version: " << dd->name;
        return;
    }
    for (auto it = dd->versions.begin(); it != dd->versions.end(); ++it) {
        if (OLModuleVersionControlPrivate::versionGreaterThan((*it)->version, version.version))
            continue;

        dd->versions.insert(it, new OLModuleInformation(version));
    }
}

void OLModuleVersionControl::removeVersion(const OLModuleInformation &version)
{
    if (version.name != dd->name) {
        qWarning() << "[OLModuleVC] remove version failed, the version: " << version.name
                   << " is not the current vc version: " << dd->name;
        return;
    }

    dd->versions.removeIf([version](OLModuleInformation *info) {
        return version.version == info->version;
    });
}
