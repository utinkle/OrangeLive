#include <QGuiApplication>
#include <QIcon>
#include <QFontDatabase>
#include <QStandardPaths>
#include <QDir>
#include <QPluginLoader>
#include <QLibraryInfo>

#include "appstartupinstance.h"

int main(int argc, char *argv[])
{
    AppStartupInstance startupInstance("OrangeLive");
    startupInstance.addModulePath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QDir::separator() + QLatin1String("startup"));
    startupInstance.addModulePath(QLibraryInfo::path(QLibraryInfo::DataPath));
    startupInstance.addModulePath(QLibraryInfo::path(QLibraryInfo::DataPath) + QDir::separator() + ORANGE_ARCH_STRING);
    return startupInstance.exec(argc, argv);
    return 0;
}
