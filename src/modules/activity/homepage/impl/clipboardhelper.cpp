/*
 * SPDX-FileCopyrightText: 2026 OpenAI
 *
 * SPDX-License-Identifier: MIT
 */

#include "clipboardhelper.h"

#include <QClipboard>
#include <QGuiApplication>

ClipboardHelper::ClipboardHelper(QObject *parent)
    : QObject(parent)
{
}

void ClipboardHelper::copyText(const QString &text)
{
    if (auto *clipboard = QGuiApplication::clipboard()) {
        clipboard->setText(text);
    }
}

#include "moc_clipboardhelper.cpp"
