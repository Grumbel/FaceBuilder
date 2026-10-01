// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#include "face.h"

Face::Face(QObject *parent)
    : QObject(parent)
    , m_parts{
          FacePart(PartType::Eye),
          FacePart(PartType::Eyebrow),
          FacePart(PartType::Glasses),
          FacePart(PartType::Ear),
          FacePart(PartType::Mouth),
          FacePart(PartType::Mouthfold),
          FacePart(PartType::Beard),
          FacePart(PartType::Nose),
          FacePart(PartType::Head),
          FacePart(PartType::Forehead),
          FacePart(PartType::Hair),
          FacePart(PartType::Hat),
      }
{
}

FacePart &Face::part(PartType t)
{
    return m_parts[static_cast<size_t>(t)];
}

const FacePart &Face::part(PartType t) const
{
    return m_parts[static_cast<size_t>(t)];
}

void Face::clearAll()
{
    for (size_t i = 0; i < static_cast<size_t>(PartType::Count); ++i) {
        auto t = static_cast<PartType>(i);
        m_parts[i].setFilename({});
        m_parts[i].setOffset(partTypeDefaultOffset(t));
        m_parts[i].resetTransform();
        emit partChanged(t);
    }
}

void Face::centerOnHead()
{
    const QPointF head = part(PartType::Head).offset();
    if (head.isNull())
        return;
    for (size_t i = 0; i < static_cast<size_t>(PartType::Count); ++i) {
        auto t = static_cast<PartType>(i);
        const QPointF o = m_parts[i].offset();
        m_parts[i].setOffset(QPointF(o.x() - head.x(), o.y() - head.y()));
        emit partChanged(t);
    }
}

void Face::setPartFilename(PartType t, const QString &path)
{
    part(t).setFilename(path);
    emit partChanged(t);
}

void Face::setPartOffset(PartType t, const QPointF &offset)
{
    part(t).setOffset(offset);
    emit partChanged(t);
}

void Face::setPartScale(PartType t, qreal scale)
{
    part(t).setScale(scale);
    emit partChanged(t);
}

void Face::setPartRotation(PartType t, qreal degrees)
{
    part(t).setRotation(degrees);
    emit partChanged(t);
}

void Face::resetPartTransform(PartType t)
{
    part(t).resetTransform();
    emit partChanged(t);
}
