// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#ifndef FACEEXPORT_H
#define FACEEXPORT_H

#include <QString>

class Face;
class QGraphicsScene;

class FaceExport
{
public:
    /** Render the scene (512×512, origin-centred) to a PNG file. */
    static bool toPng(QGraphicsScene *scene, const QString &filePath, QString *error = nullptr);

    /**
     * Write an SVG with embedded base64 PNGs for each visible part,
     * matching the original FaceBuilder SVG layout (512×512, centre origin).
     */
    static bool toSvg(const Face *face, const QString &filePath, QString *error = nullptr);
};

#endif
