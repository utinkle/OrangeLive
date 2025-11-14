#ifndef OLFONTICONENGINE_H
#define OLFONTICONENGINE_H

#include "olfonticonmanager.h"

#include <QIconEngine>
#include <QPainter>
#include <QPixmap>
#include <QFont>
#include <QColor>
#include <QList>
#include <QSize>

class OLFontIconEngine : public QIconEngine
{
public:
    OLFontIconEngine(const QString& fontFamily, QString codePoint, int size,
                       const OLFontIconManager::IconStyle& style);

    QIconEngine* clone() const override;
    void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;
    QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;

    QString key() const override;
    bool isNull() override;
    QSize actualSize(const QSize& size, QIcon::Mode mode, QIcon::State state) override;
    void virtual_hook(int id, void* data) override;

    void setColor(const QColor& color);
    void setSize(int size);
    void setFontFamily(const QString& fontFamily);
    void setCodePoint(QChar codePoint);

    QString fontFamily() const { return m_fontFamily; }
    QString codePoint() const { return m_codePoint; }
    int baseSize() const { return m_baseSize; }
    OLFontIconManager::IconStyle style() const { return m_style; }

private:
    QString m_fontFamily;
    QString m_codePoint;
    int m_baseSize;
    OLFontIconManager::IconStyle m_style;

    QColor colorForMode(QIcon::Mode mode) const;
};

#endif // OLFONTICONENGINE_H
