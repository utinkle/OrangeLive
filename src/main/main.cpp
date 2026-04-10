#include <QGuiApplication>
#include <QIcon>
#include <QFontDatabase>
#include <QStandardPaths>
#include <QDir>
#include <QPluginLoader>
#include <QLibraryInfo>
#include <QQuickWindow>

#include "appstartupinstance.h"
#include "appstartupapplicationfactory.h"

static QPointer<AppStartupInstance> g_instance = nullptr;

class OLApplicatioinFactory : public AppStartupApplicationFactory {
public:
    QGuiApplication *createApplication(int &argc, char **argv) {
        auto app = new QGuiApplication(argc, argv);
        QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);

        g_instance->addModulePath(app->applicationDirPath());
        return app;
    }
    QQmlApplicationEngine *createEngine() {
        return nullptr;
    }
};

int main(int argc, char *argv[])
{
    AppStartupInstance startupInstance("OrangeLive");
    g_instance = &startupInstance;

    OLApplicatioinFactory applicationFactory;
    startupInstance.setApplicationFactory(&applicationFactory);
    startupInstance.addModulePath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QDir::separator() + QLatin1String("startup"));
    startupInstance.addModulePath(QLibraryInfo::path(QLibraryInfo::DataPath));
    startupInstance.addModulePath(QLibraryInfo::path(QLibraryInfo::DataPath) + QDir::separator() + ORANGE_ARCH_STRING);
    return startupInstance.exec(argc, argv);
}
