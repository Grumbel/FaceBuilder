// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2005-2010 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#include "mainwindow.h"
#include "paths.h"

#include <QApplication>
#include <QGuiApplication>

int main(int argc, char *argv[])
{
    // Qt6 enables high-DPI scaling by default; keep pixmaps crisp.
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("FaceBuilder"));
    QApplication::setApplicationVersion(QStringLiteral("0.2.0"));
    QApplication::setOrganizationName(QStringLiteral("FaceBuilder"));
    QApplication::setDesktopFileName(QStringLiteral("facebuilder"));

    MainWindow window(findDataRoot());
    window.show();
    return app.exec();
}
