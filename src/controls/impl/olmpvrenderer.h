#ifndef MPVRENDERER_H
#define MPVRENDERER_H

#include <QtQuick/QQuickFramebufferObject>

#include "mpv/render_gl.h"

#include "olmpvabstractitem.h"

class MpvAbstractItem;

class MpvRenderer : public QQuickFramebufferObject::Renderer
{
public:
    explicit MpvRenderer();
    ~MpvRenderer();

    // This function is called when a new FBO is needed.
    // This happens on the initial frame.
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;

    void render() override;
    void synchronize(QQuickFramebufferObject *item) override;
    void requestUpdate();

private:
    mpv_render_context *createMpvRenderContext();
    QPointer<MpvAbstractItem> m_mpvAItem{nullptr};
    bool m_isFramebufferReady{false};
    std::shared_ptr<MpvResourceManager> m_mpvResourceManager;
};

#endif // MPVRENDERER_H
