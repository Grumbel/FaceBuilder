// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2005-2010 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#include "mainwindow.h"
#include "paths.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QGuiApplication>

int main(int argc, char *argv[])
{
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("FaceBuilder"));
    QApplication::setApplicationVersion(QStringLiteral("0.2.0"));
    QApplication::setOrganizationName(QStringLiteral("FaceBuilder"));
    QApplication::setDesktopFileName(QStringLiteral("facebuilder"));

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Compose faces from parts"));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument(QStringLiteral("file"),
                                 QStringLiteral("Face XML to open"),
                                 QStringLiteral("[file]"));
    parser.process(app);

    MainWindow window(findDataRoot());

    const QStringList pos = parser.positionalArguments();
    if (!pos.isEmpty() && QFile::exists(pos.first()))
        window.loadFaceFile(pos.first());

    window.show();
    return app.exec();
}
