#include <QGuiApplication>
#include <QIcon>
#include <QFontDatabase>
#include <QStandardPaths>
#include <QDir>
#include <QPluginLoader>
#include <QLibraryInfo>
// #include <QLoggingCategory>

#include "appstartupinstance.h"

int main(int argc, char *argv[])
{
    // QLoggingCategory::setFilterRules("qt.gui.icon.loader.*=true");

    AppStartupInstance startupInstance("OrangeLive");
    startupInstance.addModulePath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QDir::separator() + QLatin1String("startup"));
    startupInstance.addModulePath(QLibraryInfo::path(QLibraryInfo::DataPath));
    startupInstance.addModulePath(QLibraryInfo::path(QLibraryInfo::DataPath) + QDir::separator() + ORANGE_ARCH_STRING);
    return startupInstance.exec(argc, argv);
}
