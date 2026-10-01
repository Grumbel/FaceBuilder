// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#ifndef FACEPARTITEM_H
#define FACEPARTITEM_H

#include "parttypes.h"

#include <QGraphicsPixmapItem>
#include <QObject>

class FacePart;
class QGraphicsScene;

/**
 * Manages one or two QGraphicsPixmapItems for a face-part slot.
 * Mirrored types (eye, ear, …) get a second item flipped across the
 * face centre, matching the original GnomeCanvas behaviour.
 */
class FacePartItem : public QObject
{
    Q_OBJECT

public:
    FacePartItem(PartType type, QGraphicsScene *scene, QObject *parent = nullptr);
    ~FacePartItem() override;

    PartType type() const { return m_type; }

    /** Sync graphics from the model data. */
    void updateFrom(const FacePart &part);

private:
    void applyTransform(QGraphicsPixmapItem *item, const QPointF &offset,
                        qreal scale, qreal rotation, bool mirror);

    PartType m_type;
    QGraphicsScene *m_scene = nullptr;
    QGraphicsPixmapItem *m_primary = nullptr;
    QGraphicsPixmapItem *m_mirror = nullptr; // only for mirrored types
};

#endif // FACEPARTITEM_H
