// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#ifndef FACESCENE_H
#define FACESCENE_H

#include "parttypes.h"

#include <QGraphicsScene>
#include <QPointF>
#include <array>
#include <memory>

class Face;
class FacePartItem;

class FaceScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit FaceScene(Face *face, QObject *parent = nullptr);
    ~FaceScene() override;

    Face *face() const { return m_face; }
    void addGuideFrame();
    void clearFace();

    PartType currentType() const { return m_currentType; }
    void setCurrentType(PartType type);
    PartType partAt(const QPointF &scenePos) const;

signals:
    void currentTypeChanged(PartType type);
    void partMoved(PartType type, const QPointF &oldOffset, const QPointF &newOffset);

public slots:
    void onPartChanged(PartType type);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    Face *m_face = nullptr;
    std::array<std::unique_ptr<FacePartItem>, static_cast<size_t>(PartType::Count)> m_items;
    PartType m_currentType = PartType::Eye;

    bool m_dragging = false;
    PartType m_dragType = PartType::Count;
    QPointF m_dragStartScene;
    QPointF m_dragStartOffset;
};

#endif
