// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2005-2010 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QStandardPaths>

static QString findDataRoot()
{
    const QByteArray envData = qgetenv("FACEBUILDER_DATA");
    if (!envData.isEmpty())
        return QString::fromLocal8Bit(envData);

    const QDir appDir(QCoreApplication::applicationDirPath());

    // Dev tree: build/ next to source, or run from source root
    if (appDir.exists(QStringLiteral("../data")))
        return appDir.absoluteFilePath(QStringLiteral("../data"));
    if (appDir.exists(QStringLiteral("data")))
        return appDir.absoluteFilePath(QStringLiteral("data"));

    // Installed: share/facebuilder/data relative to bin/
    if (appDir.exists(QStringLiteral("../share/facebuilder/data")))
        return appDir.absoluteFilePath(QStringLiteral("../share/facebuilder/data"));

    const QString located = QStandardPaths::locate(
        QStandardPaths::GenericDataLocation,
        QStringLiteral("facebuilder/data"),
        QStandardPaths::LocateDirectory);
    if (!located.isEmpty())
        return located;

    return {};
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("FaceBuilder"));
    QApplication::setApplicationVersion(QStringLiteral("0.2.0"));
    QApplication::setOrganizationName(QStringLiteral("FaceBuilder"));

    MainWindow window(findDataRoot());
    window.show();
    return app.exec();
}
