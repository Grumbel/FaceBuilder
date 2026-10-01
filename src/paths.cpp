// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#include "paths.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QStandardPaths>

QString findDataRoot()
{
    const QByteArray envData = qgetenv("FACEBUILDER_DATA");
    if (!envData.isEmpty())
        return QDir(QString::fromLocal8Bit(envData)).absolutePath();

    const QDir appDir(QCoreApplication::applicationDirPath());

    if (appDir.exists(QStringLiteral("../data")))
        return appDir.absoluteFilePath(QStringLiteral("../data"));
    if (appDir.exists(QStringLiteral("data")))
        return appDir.absoluteFilePath(QStringLiteral("data"));
    if (appDir.exists(QStringLiteral("../share/facebuilder/data")))
        return appDir.absoluteFilePath(QStringLiteral("../share/facebuilder/data"));

    const QString located = QStandardPaths::locate(
        QStandardPaths::GenericDataLocation,
        QStringLiteral("facebuilder/data"),
        QStandardPaths::LocateDirectory);
    if (!located.isEmpty())
        return QDir(located).absolutePath();

    return {};
}

QString findExamplesDir(const QString &dataRoot)
{
    // Preferred: sibling of data/ (source tree and CMake install share/facebuilder/{data,examples})
    if (!dataRoot.isEmpty()) {
        const QString sibling = QDir(dataRoot).absoluteFilePath(QStringLiteral("../examples"));
        if (QDir(sibling).exists())
            return QDir(sibling).absolutePath();
    }

    const QDir appDir(QCoreApplication::applicationDirPath());
    if (appDir.exists(QStringLiteral("../examples")))
        return appDir.absoluteFilePath(QStringLiteral("../examples"));
    if (appDir.exists(QStringLiteral("../share/facebuilder/examples")))
        return appDir.absoluteFilePath(QStringLiteral("../share/facebuilder/examples"));

    const QString located = QStandardPaths::locate(
        QStandardPaths::GenericDataLocation,
        QStringLiteral("facebuilder/examples"),
        QStandardPaths::LocateDirectory);
    if (!located.isEmpty())
        return QDir(located).absolutePath();

    return {};
}
