// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2005-2010 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QStandardPaths>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("FaceBuilder"));
    QApplication::setApplicationVersion(QStringLiteral("0.2.0"));
    QApplication::setOrganizationName(QStringLiteral("FaceBuilder"));

    // Prefer data/ next to the executable (dev builds) or under share/.
    QString dataRoot;
    const QByteArray envData = qgetenv("FACEBUILDER_DATA");
    if (!envData.isEmpty()) {
        dataRoot = QString::fromLocal8Bit(envData);
    } else {
        const QDir appDir(QCoreApplication::applicationDirPath());
        if (appDir.exists(QStringLiteral("../data")))
            dataRoot = appDir.absoluteFilePath(QStringLiteral("../data"));
        else if (appDir.exists(QStringLiteral("data")))
            dataRoot = appDir.absoluteFilePath(QStringLiteral("data"));
        else
            dataRoot = QStandardPaths::locate(QStandardPaths::AppDataLocation,
                                              QStringLiteral("data"),
                                              QStandardPaths::LocateDirectory);
    }

    MainWindow window(dataRoot);
    window.show();
    return app.exec();
}
