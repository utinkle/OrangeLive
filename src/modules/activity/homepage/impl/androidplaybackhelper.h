/*
 * SPDX-FileCopyrightText: 2026 OpenAI
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef ANDROIDPLAYBACKHELPER_H
#define ANDROIDPLAYBACKHELPER_H

#include <QObject>
#include <qqmlintegration.h>

class AndroidPlaybackHelper : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool pictureInPictureSupported READ pictureInPictureSupported CONSTANT)

public:
    explicit AndroidPlaybackHelper(QObject *parent = nullptr);

    Q_INVOKABLE bool requestPictureInPicture();
    bool pictureInPictureSupported() const;
};

#endif // ANDROIDPLAYBACKHELPER_H
