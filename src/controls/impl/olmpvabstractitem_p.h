#ifndef OLMPVABSTRACTITEM_P_H_INCLUDED
#define OLMPVABSTRACTITEM_P_H_INCLUDED

#include "olmpvabstractitem.h"

class MpvAbstractItemPrivate
{
public:
    explicit MpvAbstractItemPrivate(MpvAbstractItem *q);

    MpvAbstractItem *q_ptr;
    MpvController *m_mpvController{nullptr};
    bool m_isRendererReady{false};
    std::shared_ptr<MpvResourceManager> m_mpvResourceManager;
};

#endif // MPVABSTRACTITEM_P_H_INCLUDED
