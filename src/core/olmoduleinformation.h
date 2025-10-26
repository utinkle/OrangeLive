#ifndef OLMODULEINFORMATION_H
#define OLMODULEINFORMATION_H

#include <QObject>

#include <items/appstartupmoduleinformation.h>

struct OLModuleInformation
{
    Q_GADGET;
    QML_ELEMENT

public:
    QString id;             // 插件ID，加载器生成，通过ID进行加载
    QString name;           // 插件的名称
    QString title;          // 插件的标题，显示在需要表明的地方
    QString titleAlias;     // 插件标题的别名
    QString description;    // 插件的详细描述
    QString productor;      // 插件的制作者
    QString workPath;       // 插件的工作目录
    QString version;        // 插件的版本号
    QStringList depends;    // 插件的依赖项
};

#endif // OLMODULEINFORMATION_H
