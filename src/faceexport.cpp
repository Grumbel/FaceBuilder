// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#include "faceexport.h"
#include "face.h"
#include "parttypes.h"

#include <QFile>
#include <QGraphicsScene>
#include <QImage>
#include <QPainter>
#include <QTransform>
#include <algorithm>

bool FaceExport::toPng(QGraphicsScene *scene, const QString &filePath, QString *error)
{
    if (!scene) {
        if (error) *error = QObject::tr("No scene");
        return false;
    }

    constexpr int size = 512;
    QImage image(size, size, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::white);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    // Scene is centred at origin with rect (-256,-256,512,512)
    scene->render(&painter, QRectF(0, 0, size, size),
                  QRectF(-256, -256, 512, 512));
    painter.end();

    if (!image.save(filePath, "PNG")) {
        if (error) *error = QObject::tr("Failed to save PNG %1").arg(filePath);
        return false;
    }
    return true;
}

static QString base64File(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
        return {};
    return QString::fromLatin1(f.readAll().toBase64());
}

bool FaceExport::toSvg(const Face *face, const QString &filePath, QString *error)
{
    QFile out(filePath);
    if (!out.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        if (error) *error = QObject::tr("Cannot write %1").arg(filePath);
        return false;
    }

    // Match original header and coordinate system.
    out.write("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
    out.write("<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\"\n");
    out.write("     xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n");
    out.write("     width=\"512\" height=\"512\">\n");

    // Export in z-order so stacking looks right in viewers that paint in document order.
    QList<PartType> order;
    for (int i = 0; i < static_cast<int>(PartType::Count); ++i)
        order.append(static_cast<PartType>(i));
    std::sort(order.begin(), order.end(), [](PartType a, PartType b) {
        return partTypeZValue(a) < partTypeZValue(b);
    });

    for (PartType t : order) {
        const FacePart &p = face->part(t);
        if (p.filename().isEmpty())
            continue;

        QImage img(p.filename());
        if (img.isNull())
            continue;

        const QString b64 = base64File(p.filename());
        if (b64.isEmpty())
            continue;

        const qreal w = img.width();
        const qreal h = img.height();
        const qreal ox = p.offset().x();
        const qreal oy = p.offset().y();
        const qreal sc = p.scale();
        const qreal rot = p.rotation();

        // Same transform chain as the original Ruby SVG export
        // (scene origin at centre of 512×512).
        auto writeImage = [&](bool mirror) {
            out.write(QStringLiteral("\n<!-- %1 -->\n").arg(partTypeName(t)).toUtf8());
            out.write("<g transform=\"");
            if (mirror) {
                // scale(-1,1) about centre then same placement
                out.write(QStringLiteral(
                    "translate(%1,%2) scale(-1,1) translate(%3,%4) "
                    "translate(%5,%6) rotate(%7) scale(%8) translate(%9,%10)")
                    .arg(w / 2).arg(h / 2)
                    .arg(w / 2 - 256).arg(-h / 2 + 256)
                    .arg(ox).arg(oy)
                    .arg(rot).arg(sc)
                    .arg(-w / 2).arg(-h / 2)
                    .toUtf8());
            } else {
                out.write(QStringLiteral(
                    "translate(%1,%2) translate(%3,%4) "
                    "translate(%5,%6) rotate(%7) scale(%8) translate(%9,%10)")
                    .arg(w / 2).arg(h / 2)
                    .arg(-w / 2 + 256).arg(-h / 2 + 256)
                    .arg(ox).arg(oy)
                    .arg(rot).arg(sc)
                    .arg(-w / 2).arg(-h / 2)
                    .toUtf8());
            }
            out.write("\">\n");
            out.write(QStringLiteral(
                "<image height=\"%1\" width=\"%2\" "
                "xlink:href=\"data:image/png;base64,%3\" x=\"0\" y=\"0\" />\n")
                .arg(int(h)).arg(int(w)).arg(b64).toUtf8());
            out.write("</g>\n");
        };

        writeImage(false);
        if (partTypeIsMirrored(t))
            writeImage(true);
    }

    out.write("</svg>\n");
    return true;
}
