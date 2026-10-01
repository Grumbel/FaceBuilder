// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#ifndef FACEPARTITEM_H
#define FACEPARTITEM_H

#include "parttypes.h"

#include <QGraphicsPixmapItem>
#include <QObject>

class FacePart;
class QGraphicsRectItem;
class QGraphicsScene;

class FacePartItem : public QObject
{
    Q_OBJECT

public:
    FacePartItem(PartType type, QGraphicsScene *scene, QObject *parent = nullptr);
    ~FacePartItem() override;

    PartType type() const { return m_type; }
    void updateFrom(const FacePart &part);
    bool containsScenePos(const QPointF &scenePos) const;
    void setSelected(bool selected);
    bool isSelected() const { return m_selected; }
    bool isVisible() const;

private:
    void applyTransform(QGraphicsPixmapItem *item, const QPointF &offset,
                        qreal scale, qreal rotation, bool mirror);
    void updateSelectionVisual();

    PartType m_type;
    QGraphicsScene *m_scene = nullptr;
    QGraphicsPixmapItem *m_primary = nullptr;
    QGraphicsPixmapItem *m_mirror = nullptr;
    QGraphicsRectItem *m_selRect = nullptr;
    bool m_selected = false;
};

#endif
