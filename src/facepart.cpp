// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#include "facepart.h"

FacePart::FacePart(PartType type)
    : m_type(type)
    , m_offset(partTypeDefaultOffset(type))
{
}

void FacePart::setFilename(const QString &path)
{
    m_filename = path;
}

void FacePart::setOffset(const QPointF &o)
{
    m_offset = o;
}

void FacePart::setScale(qreal s)
{
    if (s < 0.05)
        s = 0.05;
    if (s > 10.0)
        s = 10.0;
    m_scale = s;
}

void FacePart::setRotation(qreal degrees)
{
    m_rotation = degrees;
}

void FacePart::resetTransform()
{
    m_scale = 1.0;
    m_rotation = 0.0;
}
