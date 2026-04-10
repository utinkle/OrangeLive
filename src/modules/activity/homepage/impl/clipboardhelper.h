/*
 * SPDX-FileCopyrightText: 2026 OpenAI
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CLIPBOARDHELPER_H
#define CLIPBOARDHELPER_H

#include <QObject>
#include <qqmlintegration.h>

class ClipboardHelper : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit ClipboardHelper(QObject *parent = nullptr);

    Q_INVOKABLE void copyText(const QString &text);
};

#endif // CLIPBOARDHELPER_H
