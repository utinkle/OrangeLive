#include "olfonticonengine.h"
#include "olfonticonmanager.h"

OLFontIconEngine::OLFontIconEngine(const QString& fontFamily, QString codePoint, int size,
                                       const OLFontIconManager::IconStyle& style)
    : m_fontFamily(fontFamily)
    , m_codePoint(codePoint)
    , m_baseSize(size)
    , m_style(style)
{
}

QIconEngine* OLFontIconEngine::clone() const
{
    return new OLFontIconEngine(m_fontFamily, m_codePoint, m_baseSize, m_style);
}

void OLFontIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state)
{
    Q_UNUSED(state)

    if (m_fontFamily.isEmpty() || m_codePoint.isNull()) {
        return;
    }

    QFont font(m_fontFamily);
    int pixelSize = qMin(rect.width(), rect.height()) * 0.8;
    font.setPixelSize(pixelSize);

    painter->save();
    painter->setFont(font);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::TextAntialiasing);

    QColor color = colorForMode(mode);
    painter->setPen(color);

    painter->drawText(rect, Qt::AlignCenter, m_codePoint);
    painter->restore();
}

QPixmap OLFontIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state)
{
    if (size.isEmpty()) {
        return QPixmap();
    }

    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    paint(&painter, QRect(QPoint(0, 0), size), mode, state);

    return pixmap;
}

QString OLFontIconEngine::key() const
{
    return "OLFontIconEngine";
}

bool OLFontIconEngine::isNull()
{
    return m_fontFamily.isEmpty() || m_codePoint.isNull();
}

QSize OLFontIconEngine::actualSize(const QSize& size, QIcon::Mode mode, QIcon::State state)
{
    Q_UNUSED(mode)
    Q_UNUSED(state)

    if (size.isValid()) {
        return size;
    }

    return QSize(m_baseSize, m_baseSize);
}

void OLFontIconEngine::virtual_hook(int id, void* data)
{
    QIconEngine::virtual_hook(id, data);
}

void OLFontIconEngine::setColor(const QColor& color)
{
    m_style = OLFontIconManager::IconStyle(color);
}

void OLFontIconEngine::setSize(int size)
{
    m_baseSize = size;
}

void OLFontIconEngine::setFontFamily(const QString& fontFamily)
{
    m_fontFamily = fontFamily;
}

void OLFontIconEngine::setCodePoint(QChar codePoint)
{
    m_codePoint = codePoint;
}

QColor OLFontIconEngine::colorForMode(QIcon::Mode mode) const
{
    switch (mode) {
    case QIcon::Disabled:
        return m_style.disabledColor;
    case QIcon::Active:
        return m_style.activeColor;
    case QIcon::Selected:
        return m_style.selectedColor;
    case QIcon::Normal:
    default:
        return m_style.normalColor;
    }
}
