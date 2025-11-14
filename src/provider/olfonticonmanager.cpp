#include "olfonticonmanager.h"
#include "olfonticonengine.h"

#include <QRegularExpression>
#include <QTimer>

OLFontIconManager& OLFontIconManager::instance()
{
    static OLFontIconManager instance;
    return instance;
}

OLFontIconManager::OLFontIconManager()
    : QObject(nullptr)
{
    QTimer::singleShot(0, this, &OLFontIconManager::initialize);
}

void OLFontIconManager::initialize()
{
    QMutexLocker initLocker(&m_initMutex);

    if (m_initialized || m_initializationInProgress) {
        return;
    }

    m_initializationInProgress = true;
    QWriteLocker dataLocker(&m_dataLock);

    // 扫描可用的主题
    QStringList available = availableThemes();

    if (available.isEmpty()) {
        qWarning() << "No themes found in base path:" << m_basePath;
        m_initializationInProgress = false;
        return;
    }

    // 确定默认主题
    if (m_defaultTheme.isEmpty()) {
        if (available.contains("material")) {
            m_defaultTheme = "material";
        } else {
            m_defaultTheme = available.first();
        }
    }

    // 预加载主题
    dataLocker.unlock();
    preloadDefaultThemes();
    dataLocker.relock();

    m_initialized = true;
    m_initializationInProgress = false;

    Q_EMIT initializationComplete();
}

void OLFontIconManager::preloadDefaultThemes()
{
    QStringList themesToPreload;

    if (!m_defaultTheme.isEmpty()) {
        themesToPreload << m_defaultTheme;
    }

    QStringList allThemes = availableThemes();
    if (allThemes.size() <= 3) {
        themesToPreload << allThemes;
    }

    themesToPreload.removeDuplicates();

    int loadedCount = 0;
    for (const QString& theme : themesToPreload) {
        if (loadThemeInternal(theme, findThemePath(theme))) {
            loadedCount++;
        }
    }

    qDebug() << "Successfully preloaded" << loadedCount << "themes";
}

QIcon OLFontIconManager::getIcon(const QString& fullIconName, const QColor& color, int size)
{
    {
        QReadLocker locker(&m_dataLock);
        if (!m_initialized && !m_initializationInProgress) {
            locker.unlock();
            initialize(); // 这会获取写锁，但我们在解锁后调用
            locker.relock();
        }
    }

    ParsedIconName parsed = parseIconName(fullIconName);
    if (!parsed.isValid()) {
        qWarning() << "Invalid icon name:" << fullIconName;
        return QIcon();
    }

    if (!ensureThemeLoaded(parsed.theme)) {
        qWarning() << "Failed to load theme:" << parsed.theme << "for icon:" << fullIconName;
        return QIcon();
    }

    QString codePoint;
    QString fontFamily;
    {
        QReadLocker locker(&m_dataLock);
        if (!m_themeIcons.contains(parsed.theme) || !m_themeIcons[parsed.theme].contains(parsed.iconName)) {
            qWarning() << "Icon not found:" << parsed.iconName << "in theme:" << parsed.theme;
            return QIcon();
        }
        codePoint = m_themeIcons[parsed.theme][parsed.iconName];
        fontFamily = m_themeFontMap.value(parsed.theme);
    }

    if (fontFamily.isEmpty() || codePoint.isNull()) {
        qWarning() << "Invalid font family or code point for:" << fullIconName;
        return QIcon();
    }

    IconStyle style(color);
    return QIcon(new OLFontIconEngine(fontFamily, codePoint, size, style));
}

bool OLFontIconManager::ensureThemeLoaded(const QString& themeName)
{
    {
        QReadLocker locker(&m_dataLock);
        if (m_themeFontMap.contains(themeName)) {
            return true;
        }
    }

    // 主题未加载，需要加载
    QString themePath = findThemePath(themeName);
    if (themePath.isEmpty()) {
        qDebug() << "Theme not found:" << themeName;
        return false;
    }

    return loadThemeInternal(themeName, themePath);
}

QString OLFontIconManager::findThemePath(const QString &themeName)
{
    QString themePath = m_basePath + "/" + themeName;

    QDir themeDir;
    if (themePath.startsWith(":/")) {
        // 资源文件系统
        themeDir.setPath(":/");
        QString relativePath = themePath.mid(2);
        if (!relativePath.isEmpty()) {
            if (themeDir.cd(relativePath)) {
                return themePath;
            }
        }
    } else {
        // 文件系统
        if (QDir(themePath).exists()) {
            return themePath;
        }
    }

    // 记录已扫描的主题，避免重复扫描
    if (!m_scannedThemes.contains(themeName)) {
        m_scannedThemes.insert(themeName);
    }

    return QString();
}

bool OLFontIconManager::loadThemeInternal(const QString& themeName, const QString& themePath)
{
    {
        QReadLocker locker(&m_dataLock);
        if (m_themeFontMap.contains(themeName)) {
            return true;
        }
    }

    // 查找字体文件
    QString fontFile = findFontFile(themePath, themeName);
    if (fontFile.isEmpty()) {
        qWarning() << "No font file found for theme:" << themeName;
        Q_EMIT themeLoadFailed(themeName);
        return false;
    }

    // 加载字体
    int fontId = QFontDatabase::addApplicationFont(fontFile);
    if (fontId == -1) {
        qWarning() << "Failed to load font file:" << fontFile;
        Q_EMIT themeLoadFailed(themeName);
        return false;
    }

    QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
    if (fontFamilies.isEmpty()) {
        qWarning() << "No font families found in:" << fontFile;
        Q_EMIT themeLoadFailed(themeName);
        return false;
    }

    QString fontFamily = fontFamilies.first();

    // 加载codepoints文件
    QMap<QString, QString> icons;
    QString codePointsFile = findCodePointsFile(themePath, themeName);
    if (!codePointsFile.isEmpty()) {
        QMap<QString, QString> tempIcons;
        if (loadCodePoints(themeName, codePointsFile, tempIcons)) {
            icons = tempIcons;
        } else {
            qWarning() << "Failed to load codepoints for theme:" << themeName;
        }
    } else {
        qWarning() << "No codepoints file found for theme:" << themeName;
    }

    QWriteLocker locker(&m_dataLock);

    if (m_themeFontMap.contains(themeName)) {
        return true;
    }

    // 更新共享数据
    m_themeFontMap[themeName] = fontFamily;
    if (!icons.isEmpty()) {
        m_themeIcons[themeName] = icons;
    }

    Q_EMIT themeLoaded(themeName);
    return true;
}

QString OLFontIconManager::codePointToUnicodeString(uint codePoint)
{
    if (!isValidCodePoint(codePoint)) {
        qWarning() << "Invalid Unicode code point:" << codePoint;
        return QString();
    }

    if (codePoint <= 0xFFFF) {
        // BMP 字符，可以直接用 QChar
        return QString(QChar(static_cast<char16_t>(codePoint)));
    } else {
        // 辅助平面字符，需要使用代理对
        codePoint -= 0x10000;
        char16_t highSurrogate = static_cast<char16_t>((codePoint >> 10) + 0xD800);
        char16_t lowSurrogate = static_cast<char16_t>((codePoint & 0x3FF) + 0xDC00);
        return QString(QChar(highSurrogate)) + QString(QChar(lowSurrogate));
    }
}

bool OLFontIconManager::isValidCodePoint(uint codePoint)
{
    return codePoint <= 0x10FFFF &&
           (codePoint < 0xD800 || codePoint > 0xDFFF) && // 代理对区域无效
           codePoint != 0xFFFE && codePoint != 0xFFFF;   // 非字符
}

bool OLFontIconManager::loadCodePoints(const QString& themeName, const QString& codePointsFile,
                                        QMap<QString, QString>& outputIcons)
{
    QFile file(codePointsFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open codepoints file:" << codePointsFile;
        return false;
    }

    QTextStream in(&file);
    int loadedCount = 0;
    int highPlaneCount = 0;

    static QRegularExpression splitRegex("[\\s=]+|U\\+|0x");

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith('#')) {
            continue;
        }

        QStringList parts = line.split(splitRegex, Qt::SkipEmptyParts);
        if (parts.size() >= 2) {
            QString iconName = parts[0];
            QString codeStr = parts[1];

            bool ok = false;
            uint codePoint = 0;

            if (codeStr.startsWith("U+", Qt::CaseInsensitive)) {
                codePoint = codeStr.mid(2).toUInt(&ok, 16);
            } else if (codeStr.startsWith("0x", Qt::CaseInsensitive)) {
                codePoint = codeStr.mid(2).toUInt(&ok, 16);
            } else {
                codePoint = codeStr.toUInt(&ok, 16);
                if (!ok) {
                    codePoint = codeStr.toUInt(&ok, 10);
                }
            }

            if (ok && isValidCodePoint(codePoint)) {
                QString unicodeString = codePointToUnicodeString(codePoint);
                if (!unicodeString.isEmpty()) {
                    outputIcons[iconName] = unicodeString;
                    loadedCount++;

                    if (codePoint > 0xFFFF) {
                        highPlaneCount++;
                    }
                }
            } else {
                qWarning() << "Invalid codepoint:" << codeStr << "in line:" << line;
            }
        }
    }

    file.close();

    if (highPlaneCount > 0) {
        qDebug() << "Loaded" << loadedCount << "icons from" << codePointsFile
                 << "(" << highPlaneCount << "from supplementary planes)";
    } else {
        qDebug() << "Loaded" << loadedCount << "icons from" << codePointsFile;
    }

    return loadedCount > 0;
}

bool OLFontIconManager::hasIcon(const QString& fullIconName)
{
    ParsedIconName parsed = parseIconName(fullIconName);
    if (!parsed.isValid()) {
        return false;
    }

    if (!ensureThemeLoaded(parsed.theme)) {
        return false;
    }

    QReadLocker locker(&m_dataLock);
    return m_themeIcons.contains(parsed.theme) &&
           m_themeIcons[parsed.theme].contains(parsed.iconName);
}

QStringList OLFontIconManager::loadedThemes() const
{
    QReadLocker locker(&m_dataLock);
    return m_themeFontMap.keys();
}

QStringList OLFontIconManager::availableThemes()
{
    QDir baseDir;
    QStringList availableThemes;

    if (m_basePath.startsWith(":/")) {
        // 资源文件系统
        baseDir.setPath(":/");
        QString relativePath = m_basePath.mid(2);
        if (!relativePath.isEmpty() && baseDir.cd(relativePath)) {
            availableThemes = baseDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        }
    } else {
        // 文件系统
        baseDir.setPath(m_basePath);
        if (baseDir.exists()) {
            availableThemes = baseDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        }
    }

    // 过滤掉无效的主题（没有字体文件）
    QStringList validThemes;
    for (const QString& theme : std::as_const(availableThemes)) {
        QString themePath = m_basePath + "/" + theme;
        if (!findFontFile(themePath, theme).isEmpty()) {
            validThemes << theme;
        }
    }

    return validThemes;
}

QString OLFontIconManager::getIconCodePoint(const QString &themeName, const QString &iconName)
{
    if (!m_initialized && !m_initializationInProgress) {
        initialize();
    }

    if (!ensureThemeLoaded(themeName)) {
        return QChar();
    }

    QReadLocker locker(&m_dataLock);
    if (m_themeIcons.contains(themeName) && m_themeIcons[themeName].contains(iconName)) {
        return m_themeIcons[themeName][iconName];
    }
    return QString();
}

void OLFontIconManager::setBasePath(const QString& path)
{
    QMutexLocker initLocker(&m_initMutex);

    if (m_initialized) {
        qWarning() << "Cannot set base path after initialization";
        return;
    }

    if (m_basePath != path) {
        m_basePath = path;
        QWriteLocker dataLocker(&m_dataLock);
        m_scannedThemes.clear();
    }
}

void OLFontIconManager::setDefaultTheme(const QString& themeName)
{
    QMutexLocker initLocker(&m_initMutex);
    if (!m_initialized) {
        m_defaultTheme = themeName;
    }
}

QString OLFontIconManager::defaultTheme() const
{
    return m_defaultTheme;
}

OLFontIconManager::ParsedIconName OLFontIconManager::parseIconName(const QString& fullIconName) const
{
    ParsedIconName result;
    int colonIndex = fullIconName.indexOf(':');

    if (colonIndex != -1) {
        result.theme = fullIconName.left(colonIndex);
        result.iconName = fullIconName.mid(colonIndex + 1);
    } else {
        // 如果没有指定主题，使用默认主题或第一个可用主题
        if (!m_defaultTheme.isEmpty()) {
            result.theme = m_defaultTheme;
            result.iconName = fullIconName;
        } else if (!m_themeFontMap.isEmpty()) {
            result.theme = m_themeFontMap.firstKey();
            result.iconName = fullIconName;
        } else {
            // 没有加载任何主题，尝试使用图标名作为主题名（常见约定）
            result.theme = fullIconName.split('-').first(); // 例如 "material-home" -> "material"
            result.iconName = fullIconName;
        }
    }

    return result;
}

QString OLFontIconManager::findFontFile(const QString& themePath, const QString& themeName) const
{
    QDir themeDir;
    if (themePath.startsWith(":/")) {
        themeDir.setPath(":/");
        QString relativePath = themePath.mid(2);
        if (!relativePath.isEmpty() && !themeDir.cd(relativePath)) {
            return QString();
        }
    } else {
        themeDir.setPath(themePath);
        if (!themeDir.exists()) {
            return QString();
        }
    }

    // 优先查找与主题同名的字体文件
    QStringList possibleNames = {
        themeName + ".ttf",
        themeName + ".otf",
        "font.ttf",
        "iconfont.ttf",
        "icons.ttf"
    };

    for (const QString& fileName : possibleNames) {
        if (themeDir.exists(fileName)) {
            return themePath + "/" + fileName;
        }
    }

    // 查找任何ttf/otf文件
    QStringList fontFiles = themeDir.entryList({"*.ttf", "*.otf"}, QDir::Files);
    if (!fontFiles.isEmpty()) {
        return themePath + "/" + fontFiles.first();
    }

    return QString();
}

QString OLFontIconManager::findCodePointsFile(const QString& themePath, const QString& themeName) const
{
    QDir themeDir;
    if (themePath.startsWith(":/")) {
        themeDir.setPath(":/");
        QString relativePath = themePath.mid(2);
        if (!relativePath.isEmpty() && !themeDir.cd(relativePath)) {
            return QString();
        }
    } else {
        themeDir.setPath(themePath);
        if (!themeDir.exists()) {
            return QString();
        }
    }

    // 优先查找与主题同名的codepoints文件
    QStringList possibleNames = {
        themeName + ".codepoints",
        "codepoints.txt",
        "icons.txt",
        "mapping.txt",
        "codepoints"
    };

    for (const QString& fileName : possibleNames) {
        if (themeDir.exists(fileName)) {
            return themePath + "/" + fileName;
        }
    }

    // 查找任何文本文件
    QStringList codePointFiles = themeDir.entryList({"*.txt", "*.codepoints"}, QDir::Files);
    for (const QString& file : std::as_const(codePointFiles)) {
        if (file.contains("code", Qt::CaseInsensitive) ||
            file.contains("point", Qt::CaseInsensitive) ||
            file.contains("icon", Qt::CaseInsensitive) ||
            file.contains("map", Qt::CaseInsensitive)) {
            return themePath + "/" + file;
        }
    }

    return QString();
}

OLFontIconEngine *OLFontIconManager::createIconEngine(const QString &themeName, const QString &iconName, const QColor &color, int size)
{
    QString unicodeString;
    QString fontFamily;

    {
        QReadLocker locker(&m_dataLock);
        if (!m_themeIcons.contains(themeName) || !m_themeIcons[themeName].contains(iconName)) {
            return nullptr;
        }
        unicodeString = m_themeIcons[themeName][iconName];
        fontFamily = m_themeFontMap.value(themeName);
    }

    if (fontFamily.isEmpty() || unicodeString.isEmpty()) {
        return nullptr;
    }

    IconStyle style(color);
    return new OLFontIconEngine(fontFamily, unicodeString, size, style);
}
