#include "iconimageprovider.h"

#include <QQmlEngineExtensionPlugin>

class OrangeLiveProviderExtensionPlugin : public QQmlEngineExtensionPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlEngineExtensionInterface_iid)

public:
    void initializeEngine(QQmlEngine *engine, const char *uri) {
        engine->addImageProvider("toklive.icons", new IconImageProvider());
    }
};

#include "orangecontrolsplugin.moc"
