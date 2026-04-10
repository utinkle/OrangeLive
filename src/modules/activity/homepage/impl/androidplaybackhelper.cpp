/*
 * SPDX-FileCopyrightText: 2026 OpenAI
 *
 * SPDX-License-Identifier: MIT
 */

#include "androidplaybackhelper.h"

#ifdef Q_OS_ANDROID
#include <QJniObject>
// #include <QNativeInterface>
#endif

AndroidPlaybackHelper::AndroidPlaybackHelper(QObject *parent)
    : QObject(parent)
{
}

bool AndroidPlaybackHelper::requestPictureInPicture()
{
#ifdef Q_OS_ANDROID
    return false;
    // QJniObject activity = QNativeInterface::QAndroidApplication::context();
    // if (!activity.isValid()) {
    //     return false;
    // }

    // return activity.callMethod<jboolean>("enterPictureInPictureMode", "()Z");
#else
    return false;
#endif
}

bool AndroidPlaybackHelper::pictureInPictureSupported() const
{
#ifdef Q_OS_ANDROID
    return true;
#else
    return false;
#endif
}

#include "moc_androidplaybackhelper.cpp"
