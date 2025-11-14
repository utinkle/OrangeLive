#ifndef OLFONTICONMANAGER_H
#define OLFONTICONMANAGER_H

#include <QMap>
#include <QString>
#include <QColor>
#include <QFontDatabase>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QIconEngine>
#include <QPainter>
#include <QPixmap>
#include <QList>
#include <QMutex>
#include <QReadWriteLock>

class OLFontIconEngine;
class OLFontIconManager : public QObject
{
    Q_OBJECT

public:
    struct IconStyle {
        QColor normalColor;
        QColor disabledColor;
        QColor activeColor;
        QColor selectedColor;

        IconStyle(const QColor& baseColor = Qt::black) {
            normalColor = baseColor;
            disabledColor = baseColor.darker(150);
            activeColor = baseColor.lighter(120);
            selectedColor = baseColor.lighter(150);
        }
    };

    static OLFontIconManager& instance();

    QIcon getIcon(const QString& fullIconName, const QColor& color = Qt::black, int size = 16);
    QIcon getIcon(const QString& themeName, const QString& iconName, const QColor& color = Qt::black, int size = 16);

    bool hasIcon(const QString& fullIconName);
    bool hasIcon(const QString& themeName, const QString& iconName);
    bool hasTheme(const QString& themeName);

    QStringList loadedThemes() const;
    QStringList availableThemes();
    bool loadTheme(const QString& themeName, const QString& themePath = "");

    QStringList themeIcons(const QString& themeName);
    QString getIconCodePoint(const QString& themeName, const QString& iconName);

    void setBasePath(const QString& path);
    void setDefaultTheme(const QString& themeName);
    QString defaultTheme() const;

Q_SIGNALS:
    void themesLoaded();
    void themeLoaded(const QString& themeName);
    void themeLoadFailed(const QString& themeName);
    void initializationComplete();

private:
    OLFontIconManager();
    ~OLFontIconManager() = default;

    struct ParsedIconName {
        QString theme;
        QString iconName;
        bool isValid() const { return !theme.isEmpty() && !iconName.isEmpty(); }
    };

    static QString codePointToUnicodeString(uint codePoint);
    static bool isValidCodePoint(uint codePoint);

    void initialize();
    void preloadDefaultThemes();

    bool ensureThemeLoaded(const QString& themeName);
    QString findThemePath(const QString& themeName);
    bool loadThemeInternal(const QString& themeName, const QString& themePath);

    QString findFontFile(const QString& themePath, const QString& themeName) const;
    QString findCodePointsFile(const QString& themePath, const QString& themeName) const;
    bool loadCodePoints(const QString& themeName, const QString& codePointsFile,
                        QMap<QString, QString>& outputIcons);

    ParsedIconName parseIconName(const QString& fullIconName) const;
    OLFontIconEngine* createIconEngine(const QString& themeName, const QString& iconName,
                                      const QColor& color, int size);
private:
    QMap<QString, QString> m_themeFontMap;
    QMap<QString, QMap<QString, QString>> m_themeIcons;
    QSet<QString> m_scannedThemes;
    QString m_defaultTheme;
    QString m_basePath = ":/OL/icons";

    mutable QReadWriteLock m_dataLock;
    bool m_initialized = false;
    bool m_initializationInProgress = false;
    mutable QMutex m_initMutex;
};

#endif // OLFONTICONMANAGER_H
