// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#include "partfiles.h"

#include <QDir>
#include <QFileInfo>

QStringList listPartFiles(const QString &dataRoot, PartType type)
{
    const QString dirPath = dataRoot + QLatin1Char('/') + partTypeDirName(type);
    QDir dir(dirPath);
    if (!dir.exists())
        return {};
    const QStringList names = dir.entryList({QStringLiteral("*.png")}, QDir::Files, QDir::Name);
    QStringList out;
    out.reserve(names.size());
    for (const QString &n : names)
        out.append(dir.absoluteFilePath(n));
    return out;
}

QString cyclePartFile(const QString &dataRoot, PartType type,
                      const QString &currentAbsolute, int delta)
{
    const QStringList files = listPartFiles(dataRoot, type);
    if (files.isEmpty())
        return {};

    int index = files.indexOf(currentAbsolute);
    if (index < 0) {
        // Also try matching by file name only
        const QString base = QFileInfo(currentAbsolute).fileName();
        for (int i = 0; i < files.size(); ++i) {
            if (QFileInfo(files[i]).fileName() == base) {
                index = i;
                break;
            }
        }
    }

    if (index < 0)
        index = (delta >= 0) ? -1 : 0;

    index = (index + delta) % files.size();
    if (index < 0)
        index += files.size();
    return files[index];
}
