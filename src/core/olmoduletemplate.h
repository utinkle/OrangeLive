#ifndef OLMODULETEMPLATE_H
#define OLMODULETEMPLATE_H

#include <QQuickItem>

#include "olmoduleinformation.h"

class OLModuleTemplate : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(OLModuleInformation information READ information CONSTANT FINAL)
    Q_PROPERTY(QQuickItem * surface READ surface WRITE setSurface NOTIFY surfaceChanged FINAL)

public:
    explicit OLModuleTemplate(QQuickItem *parent = nullptr);

    OLModuleInformation information() const;
    void setInformation(const OLModuleInformation &info);

    QQuickItem *surface() const;
    void setSurface(QQuickItem *surface);

Q_SIGNALS:
    void surfaceChanged();

private:
    OLModuleInformation _information;
    QQuickItem *_surface = nullptr;
};

#endif // OLMODULETEMPLATE_H
