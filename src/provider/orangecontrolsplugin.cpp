#include "iconimageprovider.h"

#include <QQmlEngineExtensionPlugin>

class OrangeLiveProviderExtensionPlugin : public QQmlEngineExtensionPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlEngineExtensionInterface_iid)

public:
    explicit OrangeLiveProviderExtensionPlugin(QObject *parent = nullptr)
        : QQmlEngineExtensionPlugin(parent)
    {
    }

    void initializeEngine(QQmlEngine *engine, const char *uri) {
        engine->addImageProvider("ol.icons", new IconImageProvider());
    }
};

#include "orangecontrolsplugin.moc"
