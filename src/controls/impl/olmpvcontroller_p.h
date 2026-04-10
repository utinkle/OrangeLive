#ifndef OLMPVCONTROLLER_P_H_INCLUDED
#define OLMPVCONTROLLER_P_H_INCLUDED

#include "olmpvcontroller.h"

class MpvControllerPrivate
{
public:
    explicit MpvControllerPrivate(MpvController *q);

    mpv_node_list *createList(mpv_node *dst, bool is_map, int num);
    void setNode(mpv_node *dst, const QVariant &src);
    bool testType(const QVariant &v, QMetaType::Type t);
    void freeNode(mpv_node *dst);
    QVariant nodeToVariant(const mpv_node *node);

    MpvController *q_ptr;
    mpv_handle *m_mpv{nullptr};
    std::shared_ptr<MpvHandleManager> m_mpvHandleManager;
};

#endif // MPVCONTROLLER_P_H_INCLUDED
