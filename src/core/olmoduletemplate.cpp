#include "olmoduletemplate.h"

OLModuleTemplate::OLModuleTemplate(QQuickItem *parent)
    : QQuickItem(parent)
{}

OLModuleInformation OLModuleTemplate::information() const
{
    return _information;
}

void OLModuleTemplate::setInformation(const OLModuleInformation &info)
{
    if (_information.id == info.id)
        return;

    _information = info;
}

QQuickItem *OLModuleTemplate::surface() const
{
    return _surface;
}

void OLModuleTemplate::setSurface(QQuickItem *newSurface)
{
    if (_surface == newSurface)
        return;

    _surface = newSurface;
    Q_EMIT surfaceChanged();
}
