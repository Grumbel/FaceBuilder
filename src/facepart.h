// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#ifndef FACEPART_H
#define FACEPART_H

#include "parttypes.h"

#include <QPointF>
#include <QString>

/** Data for one face-part slot (no graphics). */
class FacePart
{
public:
    explicit FacePart(PartType type);

    PartType type() const { return m_type; }

    QString filename() const { return m_filename; }
    void setFilename(const QString &path);

    QPointF offset() const { return m_offset; }
    void setOffset(const QPointF &o);

    qreal scale() const { return m_scale; }
    void setScale(qreal s);

    qreal rotation() const { return m_rotation; }
    void setRotation(qreal degrees);

    void resetTransform();

private:
    PartType m_type;
    QString m_filename;   // empty = hidden / none
    QPointF m_offset;
    qreal m_scale = 1.0;
    qreal m_rotation = 0.0; // degrees, matching Qt
};

#endif // FACEPART_H
